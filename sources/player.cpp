﻿
#include"player.h"
#include"camera.h"
#include "shader.h"
#include"user.h"
#include "texture.h"
#include "Operators.h"
#include "../external/magic_enum/include/magic_enum.hpp"
#include "collision.h"
#include "noise.h"

#include <filesystem>
#include <fstream>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>


//==============================================================
// 
// 初期化
// 
//==============================================================
void Player::initialize()
{
	//パラメーターロード
	load_data_file();
	//パラメーター初期化
	position = { 0.0f, -5.0f,-20.0f };
	velocity = { 0.0f, 0.0f, 0.0f };
	//Charactorクラスのパラメーター初期化
	chara_param = param.chara_init_param;

	//体力初期化
	health = chara_param.max_health;

	jump_count = 1;
	scale.x = scale.y = scale.z = 0.05f;

	model->play_animation(PlayerAnimation::PLAYER_IDLE, true);
	damaged_function = [=](int damage, float invincible, WINCE_TYPE type)->bool {return apply_damage(damage, invincible, type); };
	state = State::IDLE;
	//attack1->particle_constants->data.particle_color = { 1.0f,0.8f,8.5f,0.7f };
	sword->initialize();

}

//==============================================================
// 
// コンストラクタ
// 
//==============================================================
Player::Player(Camera* camera)
{
	Graphics& graphics = Graphics::instance();
	//キャラクターモデル
	model = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), "./resources/Model/Player/womanParadinInplace.fbx", 30.0f);
	skill_manager = std::make_unique<SkillManager>();
	//キャラが持つ剣
	sword = std::make_unique<Sword>();
	//UI
	ui = std::make_unique<PlayerUI>();
	//攻撃時エフェクト
	for (auto& se : slash_efects)
	{
		se = std::make_unique<MeshEffect>("./resources/Effects/Meshes/eff_slash.fbx");
		se->set_material(MaterialManager::instance().mat_fire_distortion.get());
		se->set_init_scale(0.15f);
		se->set_init_life_duration(0.2f);
		se->set_init_color({ 1.8f, 1.8f, 5.2f, 0.8f });
	}

	//ヒットエフェクト
	slash_hit_line = std::make_unique<MeshEffect>("./resources/Effects/Meshes/slash_ray.fbx");
	slash_hit_line->set_material(MaterialManager::instance().mat_fire_distortion.get());
	slash_hit_line->set_init_scale(2.0f);
	slash_hit_line->set_init_life_duration(0.5f);
	slash_hit_line->set_init_color({ 2.5f,2.5f,5.9f,0.5f });


	slash_hit_particle = std::make_unique<GPU_Particles>(graphics.get_device().Get(), 3000);
	slash_hit_particle.get()->initialize();
	slash_hit_particle.get()->set_emitter_rate(5500);
	slash_hit_particle.get()->set_particle_size({ 0.1f,0.1f });
	slash_hit_particle.get()->set_emitter_life_time(0.15f);
	slash_hit_particle.get()->set_particle_streak_factor(0.03f);
	slash_hit_particle.get()->set_particle_life_time(0.4f);
	slash_hit_particle.get()->set_color({ 1.0f,2.2f,5.0f,1.0f });

	mouse = &Device::instance().get_mouse();
	game_pad = &Device::instance().get_game_pad();

	left_hand = model->get_bone_by_name("hand_l");
	right_hand = model->get_bone_by_name("hand_r");
	root = model->get_bone_by_name("pelvis");
	create_cs_from_cso(graphics.get_device().Get(), "shaders/hit_effect_emit_cs.cso", slash_hit_emit_cs.ReleaseAndGetAddressOf());
	create_cs_from_cso(graphics.get_device().Get(), "shaders/hit_effect_update_cs.cso", slash_hit_update_cs.ReleaseAndGetAddressOf());

	initialize();
}

//==============================================================
// 
//デストラクタ
// 
//==============================================================
Player::~Player()
{
}


//==============================================================
// 
//更新処理
// 
//==============================================================
void Player::update(float elapsed_time, Camera* camera)
{
	Graphics& graphics = Graphics::instance();
	//更新処理
	(this->*p_update)(elapsed_time, camera);

	if (game_pad->get_button_down() & GamePad::BTN_Y)
	{
		camera->set_lock_on();
	}
	//ガード入力
	input_gurd();

	//プレイヤーの正面情報を更新
	forward = Math::get_posture_forward(orientation);
	//無敵時間の更新
	update_invicible_timer(elapsed_time);
	//シールド更新
	update_guard_system(elapsed_time);
	for (auto& se : slash_efects)
	{
		se->update(elapsed_time);
		if (se->get_active())
		{
			se->set_scale(se->get_scale().x + (0.9f * elapsed_time));
		}

	}
	slash_hit_line->update(elapsed_time);
	slash_hit_particle.get()->update(graphics.get_dc().Get(), elapsed_time, slash_hit_update_cs.Get());
	//スキルの更新
	skill_manager.get()->update(elapsed_time);

	//モデルのアニメーション更新
	model->update_animation(elapsed_time);

	//ソード更新
	{
		sword->update(elapsed_time);

		//通常攻撃状態じゃなければ当たり判定を切る
		if (state != State::NORMAL_ATTACK)
		{
			attack_sword_param.is_attack = false;
		}
	}

	collider.start = position;
	collider.end = { position.x,position.y + chara_param.height, position.z };
	collider.radius = 1.0f;
	//skill系仮置き

	//スキル選択中カメラ操作ストップ
	camera->set_camera_operate_stop(skill_manager.get()->is_selecting_skill());
	//UI
	ui->set_hp_percent(get_hp_percent());
	ui->update(elapsed_time);
}
//==============================================================
// 
//描画処理（ディファード）
// 
//==============================================================
void Player::render_d(float elapsed_time, Camera* camera)
{
	Graphics& graphics = Graphics::instance();
	DirectX::XMFLOAT4X4 sword_hand_mat = {};
	//剣のトランスフォーム更新
	model->fech_bone_world_matrix(transform, right_hand, &sword_hand_mat);
	sword->set_parent_transform(sword_hand_mat);

	//自機モデルのトランスフォーム更新
	transform = Math::calc_world_matrix(scale, orientation, position);
	graphics.shader->render(graphics.get_dc().Get(), model.get(), camera->get_view(), camera->get_projection(), transform);
	//剣描画
	sword->render();
}
//==============================================================
// 
//描画処理（フォワード）
// 
//==============================================================
void Player::render_f(float elapsed_time, Camera* camera)
{
	Graphics& graphics = Graphics::instance();
	for (auto& se : slash_efects)
	{
		se->render(camera);
	}
	slash_hit_line->render(camera);
	slash_hit_particle->render(graphics.get_dc().Get(), graphics.get_device().Get());
	skill_manager.get()->render(camera);
	slash_hit_particle->debug_gui("slash_hit");
	//デバッグGUI描画
	debug_gui(camera);

}
//==============================================================
// 
//描画処理（シャドウ）
// 
//==============================================================
void Player::render_s(float elapsed_time, Camera* camera)
{
	Graphics& graphics = Graphics::instance();
	graphics.shader->render(graphics.get_dc().Get(), model.get(), transform);
}

//==============================================================
// 
//描画処理（UI）
// 
//==============================================================
void Player::render_ui(float elapsed_time)
{
	//プレイヤーのUI
	ui->render();
	//スキルのUI
	skill_manager.get()->ui_render(elapsed_time);
}



//==============================================================
// 
//移動ベクトル処理（コントローラー）
// 
//==============================================================
const DirectX::XMFLOAT3 Player::get_move_vec(Camera* camera) const
{
	//入力情報を取得
	float ax = game_pad->get_axis_LX();
	float ay = game_pad->get_axis_LY();

	//コントローラーのスティック入力値が一定以下なら入力をはじく
	//if (fabs(ax) > 0.0f && fabs(ax) < 0.5f)  ax += -1.4f * (ax * ax) + 0.5f;
	//if (fabs(ay) > 0.0f && fabs(ay) < 0.5f)  ay += -1.4f * (ay * ay) + 0.5f;
	if (fabs(ax) < 0.3f)  ax = 0.0f;
	if (fabs(ay) < 0.3f)  ay = 0.0f;
	//カメラ右方向ベクトルをXZ単位ベクトルに変換
	float camera_forward_x = camera->get_forward().x;
	float camera_forward_z = camera->get_forward().z;
	float camera_forward_lengh = sqrtf(camera_forward_x * camera_forward_x + camera_forward_z * camera_forward_z);
	if (camera_forward_lengh > 0.0f)
	{
		camera_forward_x /= camera_forward_lengh;
		camera_forward_z /= camera_forward_lengh;
	}

	float camera_right_x = camera->get_right().x;
	float camera_right_z = camera->get_right().z;
	float camera_right_lengh = sqrtf(camera_right_x * camera_right_x + camera_right_z * camera_right_z);

	if (camera_right_lengh > 0.0f)
	{
		camera_right_x /= camera_right_lengh;
		camera_right_z /= camera_right_lengh;
	}

	DirectX::XMFLOAT3 vec{};
	vec.x = (camera_forward_x * ay) + (camera_right_x * ax);
	vec.z = (camera_forward_z * ay) + (camera_right_z * ax);

	return vec;
}

//==============================================================
// 
//移動入力処理
// 
//==============================================================
bool Player::input_move(float elapsedTime, Camera* camera)
{
	//進行ベクトル取得
	const DirectX::XMFLOAT3 move_vec = get_move_vec(camera);

	//移動処理
	Move(move_vec.x, move_vec.z, chara_param.move_speed);
	Turn(elapsedTime, move_vec, chara_param.turn_speed, orientation);

	return move_vec.x != 0.0f || move_vec.y != 0.0f || move_vec.z != 0.0f;
}

//移動速度と回転速度を制限した移動入力
bool Player::input_move(float elapsedTime, Camera* camera, float restriction_move, float restriction_turn)
{
	//進行ベクトル取得
	const DirectX::XMFLOAT3 move_vec = get_move_vec(camera);

	//移動処理
	Move(move_vec.x, move_vec.z, chara_param.move_speed / restriction_move);
	Turn(elapsedTime, move_vec, chara_param.turn_speed / restriction_turn, orientation);

	return move_vec.x != 0.0f || move_vec.y != 0.0f || move_vec.z != 0.0f;
	return false;
}

//==============================================================
// 
//ジャンプ入力処理
// 
//==============================================================
void Player::input_jump()
{
	if (game_pad->get_button_down() & GamePad::BTN_A) //スペースを押したらジャンプ
	{
		if (jump_count < jump_limit)
		{
			transition_jump_state();
			Jump(param.jump_speed);
			is_ground = false;//ジャンプしても地面についているというありえない状況を回避するため

			++jump_count;
		}
	}
}
//==============================================================
// 
//回避入力処理
// 
//==============================================================
void Player::input_avoidance()
{
	if (game_pad->get_button_down() & GamePad::BTN_B)
	{
		transition_avoidance_state();
	}
}

//==============================================================
// 
//サポートスキル発動処理
// 
//==============================================================
void Player::input_chant_support_skill(Camera* camera)
{
	if (game_pad->get_button() & GamePad::BTN_LEFT_TRIGGER) //左トリガーでサポートスキル発動
	{
		switch (skill_manager->get_selected_sup_skill_type())
		{
		case SP_SKILLTYPE::PHYSICAL_UP:
			if (skill_manager->chant_physical_up(&position, &chara_param.move_speed, &param.jump_speed))
			{
				transition_magic_buff_state();//状態遷移
			}
			break;
		case SP_SKILLTYPE::REGENERATE:
			if (skill_manager->chant_regenerate(&position, &health, get_max_health()))
			{
				transition_attack_slash_up_state();
			}

			break;
		case SP_SKILLTYPE::RESTRAINNT:
			transition_attack_pull_slash_state();
			break;
		case SP_SKILLTYPE::TEST:
			transition_attack_pull_slash_state();
			break;
		default:
			break;
		}
	}
}

//==============================================================
// 
//攻撃スキル発動処理
// 
//==============================================================
void Player::input_chant_attack_skill(Camera* camera)
{
	model->fech_by_bone(transform, left_hand, left_hand_pos);
	if (game_pad->get_button() & GamePad::BTN_RIGHT_TRIGGER)  //右トリガーで攻撃スキル発動
	{
		switch (skill_manager->get_selected_atk_skill_type())
		{
		case ATK_SKILLTYPE::MAGICBULLET:

			//スキルを発動できた場合遷移
			if (skill_manager->chant_magic_bullet(&left_hand_pos, &forward))
			{
				transition_attack_bullet_state();
			}
			break;
		case ATK_SKILLTYPE::SPEARS_SEA:
			if (is_ground)
			{
				if (skill_manager->chant_spear_sea(position, position))
				{
					transition_attack_ground_state();
				}
			}
			break;
		case ATK_SKILLTYPE::SLASH_WAVE:
			if (skill_manager->chant_slash_wave(&left_hand_pos, &forward))
			{
				transition_r_attack_spring_slash_state();
			}
			break;
		case ATK_SKILLTYPE::LIGHTNING_RAIN:
			if (skill_manager->chant_lightning_rain(position, camera->get_lock_on_target()))
			{
				transition_attack_pull_slash_state();//状態遷移
			}

			break;
		default:
			break;
		}


	}
}
//==============================================================
// 
//スキルの当たり判定処理
// 
//==============================================================
void Player::judge_skill_collision(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera)
{
	skill_manager->judge_magic_bullet_vs_enemy(object_colider, damaged_func, camera);
	skill_manager->judge_spear_sea_vs_enemy(object_colider, damaged_func, camera);
	skill_manager->judge_lightning_rain_vs_enemy(object_colider, damaged_func, camera);
	skill_manager->judge_slash_wave_vs_enemy(object_colider, damaged_func, camera);
}
//==============================================================
// 
//自分と敵の体の当たり判定処理
// 
//==============================================================
void Player::calc_collision_vs_enemy(Capsule collider, float collider_height)
{
	Collision::cylinder_vs_cylinder(collider.start, collider.radius, collider_height, position, chara_param.radius, chara_param.height, &position);
}
//==============================================================
// 
//自分の攻撃と敵の当たり判定処理
// 
//==============================================================
void Player::calc_attack_vs_enemy(Capsule collider, AddDamageFunc damaged_func, Camera* camera)
{
	//剣の攻撃中のみ当たり判定
	if (attack_sword_param.is_attack)
	{
		if (Collision::capsule_vs_capsule(collider.start, collider.end, collider.radius, sword->get_collision().start, sword->get_collision().end, sword->get_collision().radius))
		{
			//攻撃対象に与えるダメージ量と無敵時間
			if (damaged_func(attack_sword_param.power, attack_sword_param.invinsible_time, WINCE_TYPE::NONE))
			{
				//攻撃が当たったらスキルのクールタイムを短縮させる
				skill_manager->cool_time_reduction();
				camera->set_camera_shake(attack_sword_param.camera_shake);
				//ヒットストップ
				camera->set_hit_stop(attack_sword_param.hit_stop);
				game_pad->set_vibration(attack_sword_param.hit_viberation.l_moter, attack_sword_param.hit_viberation.r_moter, attack_sword_param.hit_viberation.vibe_time);
				//ヒットエフェクト再生
				if (!slash_hit_line->get_active())
				{
					//ヒットエフェクト
					slash_hit_line->play({ sword->get_collision().end });
					slash_hit_line->set_rotate_quaternion(MeshEffect::AXIS::UP, Noise::instance().random_range(0, 90));
					slash_hit_line->set_rotate_quaternion(MeshEffect::AXIS::FORWARD, Noise::instance().random_range(0, 90));

					slash_hit_particle->set_emitter_pos({ sword->get_collision().end });

					DirectX::XMFLOAT4X4 sword_hand_mat = {};
					//剣のトランスフォーム更新
					model->fech_bone_world_matrix(transform, right_hand, &sword_hand_mat);
					DirectX::XMFLOAT3 vec = Math::get_posture_forward(slash_hit_line->get_orientation());
					const float slash_power = 70;
					//slash_hit_particle->set_emitter_velocity(Math::vector_scale(vec, slash_power));
					slash_hit_particle->launch_emitter(slash_hit_emit_cs.Get());
				}
				//test_slash_hit->play(attack_sword_param.collision.end,100.0f);
			}
		}
	}
}
//==============================================================
// 
//ガード更新
// 
//==============================================================

void Player::update_guard_system(float elapsed_time)
{

	if (shield_param.is_shield)
	{
		shield_param.shield_time += elapsed_time;
	}
	//シールドが割れた際の処理
	if (shield_param.shield_hp < 0)
	{
		shield_param.is_shield = false;
		shield_param.shieldable = false;
		shield_param.is_break_shield = true;
	}
	//シールド破壊時
	if (shield_param.is_break_shield)
	{
		//ガードHPがMAXに行くまで回復
		if (shield_param.shield_hp < shield_param.SHIELD_HP_MAX)
		{
			shield_param.shield_hp += shield_param.recast_rate * elapsed_time;
		}
		else
		{
			shield_param.shieldable = true;
			shield_param.is_break_shield = false;
		}
	}
}

//==============================================================
// 
//着地処理
// 
//==============================================================
void Player::on_landing()
{
	jump_count = 0;
	//transition_landing_state();
	if (velocity.y < gravity * 30.0f)// 坂道歩いているときは遷移しない程度に調整
	{
		// 着地ステートへ遷移
		transition_idle_state();
		velocity = { 0,0,0 };
	}

}

//==============================================================
// 
//死亡処理
// 
//==============================================================
void Player::on_dead()
{
	initialize();
}
//==============================================================
// 
//ダメージを受けた際の処理
// 
//==============================================================
void Player::on_damaged(WINCE_TYPE type)
{
	switch (type)
	{
	case WINCE_TYPE::NONE:
		break;
	case WINCE_TYPE::SMALL:
		transition_damage_front_state();
		break;
	case WINCE_TYPE::BIG:
		break;
	default:
		break;
	}

}

bool Player::apply_damage(int damage, float invincible_time, WINCE_TYPE type)
{
	//ダメージが0の場合は健康状態を変更する必要がない
	if (damage == 0)return false;

	//死亡している場合は健康状態を変更しない
	if (health <= 0)return false;


	if (invincible_timer > 0.0f)return false;

	//ガード判定
	if (judge_gurd())
	{
		shield_param.shield_hp -= damage;
	}


	//無敵時間設定
	invincible_timer = invincible_time;
	//ダメージ処理
	health -= damage;

	//死亡通知
	if (health <= 0)
	{
		on_dead();
	}
	else//ダメージ通知
	{
		on_damaged(type);
	}

	//健康状態が変更した場合はtrueを返す
	return true;
}

//==============================================================
// 
//ルートモーション
// 
//==============================================================
//アニメーションのボーンの位置差分から計算
void Player::root_motion(DirectX::XMFLOAT3 dir, float speed)
{
	float root_defference_length = model->root_defference_length_next_frame(root);
	velocity.x = dir.x * (root_defference_length * speed);
	velocity.z = dir.z * (root_defference_length * speed);

}
//手動（ボーンの処理がうまくいかなかったため）
void Player::root_motion_manual(DirectX::XMFLOAT3 dir, float speed)
{
	velocity.x = dir.x * speed;
	velocity.z = dir.z * speed;
}

//少し浮遊する
bool Player::floating()
{
	//落下中なら
	if (velocity.y < 0)
	{
		//落下速度を弱める
		velocity.y /= param.floating_value;
		return true;
	}
	//浮遊中でない
	return false;
}

void Player::input_gurd()
{
	if (game_pad->get_button_down() & GamePad::BTN_LEFT_SHOULDER)
	{
		if (shield_param.shieldable)
		{
			shield_param.is_shield = true;
			//transition_guard_state();
		}
		else
		{

		}
	}
	else
	{
		shield_param.is_shield = false;
		shield_param.shield_time = false;
	}
}

bool Player::judge_gurd()
{
	if (shield_param.is_shield)
	{
		//ジャストガード判定
		if (shield_param.shield_time < JUST_GURD_TIME)
		{
			//ジャストガード成功
			invincible_timer = 0.2f;
			return true;
		}
		return true;
	}
	return false;
}


void Player::load_data_file()
{
	// Jsonファイルから値を取得
	std::filesystem::path path = file_path;
	path.replace_extension(".json");
	if (std::filesystem::exists(path.c_str()))
	{
		std::ifstream ifs;
		ifs.open(path);
		if (ifs)
		{
			cereal::JSONInputArchive o_archive(ifs);
			o_archive(param);
		}
	}
}

void Player::save_data_file()
{
	//ベースクラスの初期化パラメーター情報を更新
	param.chara_init_param = chara_param;
	// Jsonファイルから値を取得
	std::filesystem::path path = file_path;
	path.replace_extension(".json");
	std::ofstream ifs;
	ifs.open(path);
	if (ifs)
	{
		cereal::JSONOutputArchive o_archive(ifs);
		o_archive(param);
	}

}

//==============================================================
// 
//デバッグGUI表示
// 
//==============================================================
void Player::debug_gui(Camera* camera)
{
#ifdef USE_IMGUI
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	imgui_menu_bar("Charactor", "player", display_player_imgui);

	if (display_player_imgui)
	{

		if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
		{
			//ヒットエフェクト
			ImGui::Begin("slash_hit");
			if (ImGui::Button("slash_emit"))
			{

				slash_hit_particle.get()->launch_emitter(slash_hit_emit_cs.Get());
			}
			ImGui::End();


			//カメラ
			//トランスフォーム
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{

				//位置
				ImGui::DragFloat3("Position", &position.x);
				//回転
				DirectX::XMFLOAT3 forward;
				DirectX::XMStoreFloat3(&forward, Math::get_posture_forward_vec(orientation));
				ImGui::DragFloat3("forward", &forward.x);
				ImGui::DragFloat4("ori", &orientation.x);
				std::string state_name;
				state_name = magic_enum::enum_name<State>(state);
				ImGui::Text(state_name.c_str());
				ImGui::DragFloat3("velocity:", &velocity.x);
			}
			if (ImGui::CollapsingHeader("Param", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::DragFloat("height", &chara_param.height);
				ImGui::DragInt("max_health", &chara_param.max_health);
				ImGui::DragInt("health", &health);
				ImGui::DragFloat("radius", &chara_param.radius);
				ImGui::DragFloat("gravity", &gravity);
				ImGui::DragFloat("floating_value", &param.floating_value);
				ImGui::DragFloat("invinsible_timer", &invincible_timer);
				ImGui::DragFloat("TurnSpeed", &chara_param.turn_speed, 0.1f);
				ImGui::DragFloat("MoveSpeed", &chara_param.move_speed, 0.1f);
				ImGui::DragFloat("avoidance_speed", &param.avoidance_speed);
				ImGui::DragFloat("friction", &chara_param.friction);
				ImGui::DragFloat("acceleration", &chara_param.acceleration);
				ImGui::DragFloat("jump_speed", &param.jump_speed);
				ImGui::DragFloat("air_control", &chara_param.air_control);
				ImGui::Checkbox("is_ground", &is_ground);
				float control_x = game_pad->get_axis_LX();
				float control_y = game_pad->get_axis_LY();
				ImGui::DragFloat("control_x", &control_x);
				ImGui::DragFloat("control_y", &control_y);
			}
			if (ImGui::CollapsingHeader("AttackCameraShake", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::Button("set_vibration"))
				{
					game_pad->set_vibration(1, 1, 0.5);

				}
				if (ImGui::Button("load"))
				{
					load_data_file();
				}
				ImGui::Separator();
				if (ImGui::Button("save"))
				{
					save_data_file();
				}
				ImGui::Text("attack_pparam");
				if (ImGui::CollapsingHeader("combo1"))
				{
					ImGui::DragInt("combo1_power", &param.combo_1.power, 0.1f);
					ImGui::DragFloat("combo1_invinsible_time", &param.combo_1.invinsible_time, 0.1f);

					ImGui::Text("combo1_camera_shake");
					ImGui::DragFloat("combo1_shake_x", &param.combo_1.camera_shake.max_x_shake, 0.1f);
					ImGui::DragFloat("combo1_shake_y", &param.combo_1.camera_shake.max_y_shake, 0.1f);
					ImGui::DragFloat("combo1_time", &param.combo_1.camera_shake.time, 0.1f);
					ImGui::DragFloat("combo1_smmoth", &param.combo_1.camera_shake.shake_smoothness, 0.1f, 0.1f, 1.0f);

					ImGui::Text("hit_stop");
					ImGui::DragFloat("combo1_stop_time", &param.combo_1.hit_stop.time, 0.1f);
					ImGui::DragFloat("combo1_stopping_strength", &param.combo_1.hit_stop.stopping_strength, 0.1f);
					ImGui::DragFloat("combo1_hit_viberation.l_moter", &param.combo_1.hit_viberation.l_moter, 0.1f);
					ImGui::DragFloat("combo1_hit_viberation.r_moter", &param.combo_1.hit_viberation.r_moter, 0.1f);
					ImGui::DragFloat("combo1_vibe_time", &param.combo_1.hit_viberation.vibe_time, 0.1f);
				}
				if (ImGui::CollapsingHeader("combo2"))
				{
					ImGui::DragInt("combo2_power", &param.combo_2.power, 0.1f);
					ImGui::DragFloat("combo2_invinsible_time", &param.combo_2.invinsible_time, 0.1f);

					ImGui::Text("combo2_camera_shake");
					ImGui::DragFloat("combo2_shake_x", &param.combo_2.camera_shake.max_x_shake, 0.1f);
					ImGui::DragFloat("combo2_shake_y", &param.combo_2.camera_shake.max_y_shake, 0.1f);
					ImGui::DragFloat("combo2_time", &param.combo_2.camera_shake.time, 0.1f);
					ImGui::DragFloat("combo2_smmoth", &param.combo_2.camera_shake.shake_smoothness, 0.1f, 0.1f, 1.0f);
					ImGui::Text("hit_stop");
					ImGui::DragFloat("combo2_stop_time", &param.combo_2.hit_stop.time, 0.1f);
					ImGui::DragFloat("combo2_stopping_strengthy", &param.combo_2.hit_stop.stopping_strength, 0.1f);
					ImGui::DragFloat("combo2_hit_viberation.l_moter", &param.combo_2.hit_viberation.l_moter, 0.1f);
					ImGui::DragFloat("combo2_hit_viberation.r_moter", &param.combo_2.hit_viberation.r_moter, 0.1f);
					ImGui::DragFloat("combo2_vibe_time", &param.combo_2.hit_viberation.vibe_time, 0.1f);


				}
				if (ImGui::CollapsingHeader("combo3"))
				{
					ImGui::DragInt("combo3_power", &param.combo_3.power, 0.1f);
					ImGui::DragFloat("combo3_invinsible_time", &param.combo_3.invinsible_time, 0.1f);

					ImGui::Text("combo3_camera_shake");
					ImGui::DragFloat("combo3_shake_x", &param.combo_3.camera_shake.max_x_shake, 0.1f);
					ImGui::DragFloat("combo3_shake_y", &param.combo_3.camera_shake.max_y_shake, 0.1f);
					ImGui::DragFloat("combo3_time", &param.combo_3.camera_shake.time, 0.1f);
					ImGui::DragFloat("combo3_smmoth", &param.combo_3.camera_shake.shake_smoothness, 0.1f, 0.1f, 1.0f);
					ImGui::Text("hit_stop");
					ImGui::DragFloat("combo3_stop_time", &param.combo_3.hit_stop.time, 0.1f);
					ImGui::DragFloat("combo3_stopping_strength", &param.combo_3.hit_stop.stopping_strength, 0.1f);
					ImGui::DragFloat("combo3_hit_viberation.l_moter", &param.combo_3.hit_viberation.l_moter, 0.1f);
					ImGui::DragFloat("combo3_hit_viberation.r_moter", &param.combo_3.hit_viberation.r_moter, 0.1f);
					ImGui::DragFloat("combo3_vibe_time", &param.combo_3.hit_viberation.vibe_time, 0.1f);


				}
			}
			if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen))
			{
				const char* anime_item[] = { "IDLE","MOVE","JUMP","FALL","LANDING" };
				static int item_current = 0;
				static bool loop = false;
				ImGui::Combo("anime", &item_current, anime_item, IM_ARRAYSIZE(anime_item)); ImGui::Checkbox("is_loop", &loop);
				if (ImGui::Button("play", { 80,20 }))
				{
					model->play_animation(item_current, loop, 0.1f);
				}
				string s;
				ImGui::DragInt("index", &model->anime_param.current_index);
				ImGui::DragInt("frame_index", &model->anime_param.frame_index);
				ImGui::Checkbox("end_flag", &model->anime_param.end_flag);
				ImGui::DragFloat("current_time", &model->anime_param.current_time);
				ImGui::DragFloat("playback_speed", &model->anime_param.playback_speed, 0.1f);
				ImGui::DragFloat("blend_time", &model->anime_param.blend_time);
				ImGui::DragFloat("sampling", &model->anime_param.animation.sampling_rate);

			}

		}
		ImGui::End();

	}



	skill_manager.get()->debug_gui();
	slash_hit_line->debug_gui("test_slash_hit");
#endif // USE_IMGUI

}

