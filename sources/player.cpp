
#include"player.h"
#include"camera.h"
#include "shader.h"
#include"user.h"
#include "texture.h"
#include "Operators.h"
#include "../external/magic_enum/include/magic_enum.hpp"
#include "collision.h"
#include "noise.h"
//==============================================================
// 
// 初期化
// 
//==============================================================
void Player::initialize()
{
	//パラメーター初期化
	position = { 0.0f, 10.0f,-20.0f };
	velocity = { 0.0f, 0.0f, 0.0f };
	move_speed = 30.0f;
	turn_speed = DirectX::XMConvertToRadians(360);
	health = 1000;
	max_health = 1000;
	invincible_timer = 0.0f;
	jump_count = 1;
	jump_speed = 27.0f;
	scale.x = scale.y = scale.z = 0.05f;
	radius = 1.0f;
	height = 7.0f;
	friction = 2.0f;
	acceleration = 15.0f;

	model->play_animation(PlayerAnimation::PLAYER_IDLE, true);
	damaged_function = [=](int damage, float invincible, WINCE_TYPE type)->void {apply_damage(damage, invincible, type); };
	state = State::IDLE;
	attack1->particle_constants->data.particle_color = { 1.0f,0.8f,8.5f,0.7f };
	sword->initialize();

}

//==============================================================
// 
// コンストラクタ
// 
//==============================================================
Player::Player(Graphics& graphics, Camera* camera)
{
	//キャラクターモデル
	model = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), "./resources/Model/Player/womanParadinInplace.fbx", 30.0f);
	skill_manager = std::make_unique<SkillManager>(graphics);
	//キャラが持つ剣
	sword = std::make_unique<Sword>(graphics);
	//UI
	ui = std::make_unique<PlayerUI>(graphics);
	//攻撃時エフェクト
	slash_efect = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_slash.fbx");
	slash_efect->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill3_output.png");
	slash_efect->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/T_Perlin_Noise_M.tga");
	slash_efect->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga");
	slash_efect->create_pixel_shader(graphics.get_device().Get(), "./shaders/fire_distortion.cso");
	slash_efect->set_scale(0.15f);
	slash_efect->constants->data.particle_color = { 1.8f,1.8f,5.2f,0.8f };

	//ヒットエフェクト
	test_slash_hit = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/slash_ray.fbx");
	test_slash_hit->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill3_output.png");
	test_slash_hit->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/T_Perlin_Noise_M.tga");
	test_slash_hit->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga");
	test_slash_hit->create_pixel_shader(graphics.get_device().Get(), "./shaders/fire_distortion.cso");
	test_slash_hit->set_scale(2.0f);
	test_slash_hit->constants->data.particle_color = { 2.5f,2.5f,5.9f,0.5f };

	
	attack1 = std::make_unique<GPU_Particles>(graphics.get_device().Get(),200000);
	attack1.get()->initialize(graphics);
	mouse = &Device::instance().get_mouse();
	game_pad = &Device::instance().get_game_pad();

	left_hand = model->get_bone_by_name("pelvis");
	right_hand = model->get_bone_by_name("hand_r");
	root = model->get_bone_by_name("pelvis");
	create_cs_from_cso(graphics.get_device().Get(), "shaders/boss_attack1_emit_cs.cso", emit_cs.ReleaseAndGetAddressOf());
	create_cs_from_cso(graphics.get_device().Get(), "shaders/boss_attack1_update_cs.cso", update_cs.ReleaseAndGetAddressOf());

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
void Player::update(Graphics& graphics, float elapsed_time, Camera* camera)
{
	//更新処理
	(this->*p_update)(graphics, elapsed_time, camera);
	
	//オブジェクト行列を更新
	//無敵時間の更新
	update_invicible_timer(elapsed_time);
	slash_efect->update(graphics,elapsed_time);
	test_slash_hit->update(graphics,elapsed_time);
	attack1.get()->update(graphics.get_dc().Get(),elapsed_time, update_cs.Get());
	skill_manager.get()->update(graphics, elapsed_time);
	model->update_animation(elapsed_time);
	
	//ソード更新
	{
		sword->update(graphics, elapsed_time);

		attack_sword_param.collision.start = sword->get_collision().start;
		attack_sword_param.collision.end = sword->get_collision().end;
		attack_sword_param.collision.radius = sword->get_collision().radius;
	}
	/*仮置き*/
	collider.start = position;
	collider.end = { position.x,position.y + height, position.z };
	collider.radius = 1.0f;
	//skill系仮置き
	
	//スキル選択中カメラ操作ストップ
	camera->set_camera_operate_stop(skill_manager.get()->is_selecting_skill());
	//UI
	ui->set_hp_percent(get_hp_percent());
	//ui->update(graphics, elapsed_time);
}
//==============================================================
// 
//描画処理（ディファード）
// 
//==============================================================
void Player::render_d(Graphics& graphics, float elapsed_time, Camera* camera)
{
	// 拡大縮小（S）・回転（R）・平行移動（T）行列を計算する
	// スタティックメッシュ
	//ルートモーションをしている最中は
	DirectX::XMFLOAT4X4 sword_hand_mat = {};
	//剣のトランスフォーム更新
	model->fech_bone_world_matrix(transform, right_hand, &sword_hand_mat);
	sword->set_parent_transform(sword_hand_mat);

	//自機モデルのトランスフォーム更新
	transform = Math::calc_world_matrix(scale, orientation, position);
	graphics.shader->render(graphics.get_dc().Get(), model.get(), transform);
	//剣描画
	sword->render(graphics);
}
//==============================================================
// 
//描画処理（フォワード）
// 
//==============================================================
void Player::render_f(Graphics& graphics, float elapsed_time, Camera* camera)
{
	slash_efect->render(graphics);
	test_slash_hit->render(graphics);
	attack1->render(graphics.get_dc().Get(),graphics.get_device().Get());
	skill_manager.get()->render(graphics);
	attack1->debug_gui("player_attack1");
	//デバッグGUI描画
	debug_gui(graphics);

}

//==============================================================
// 
//描画処理（UI）
// 
//==============================================================
void Player::render_ui(Graphics& graphics, float elapsed_time)
{
	//プレイヤーのUI
	ui->render(graphics);
	//スキルのUI
	skill_manager.get()->ui_render(graphics, elapsed_time);
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
//魔法
// 
//==============================================================
void Player::Attack(Graphics& graphics, float elapsed_time)
{
	DirectX::XMFLOAT3 emit_pos = position + Math::vector_scale(Math::get_posture_forward_vec(orientation),14);
	emit_pos.y = position.y + 3.0f;
	attack1.get()->set_emitter_pos(emit_pos);
	attack1.get()->set_emitter_rate(150);
	attack1.get()->set_particle_size({0.1f,0.1f});
	attack1.get()->set_emitter_life_time(0.2f);
	attack1.get()->launch_emitter( emit_cs);
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
	Move(move_vec.x, move_vec.z, this->move_speed);
	Turn(elapsedTime, move_vec, this->turn_speed, orientation);

	return move_vec.x != 0.0f || move_vec.y != 0.0f || move_vec.z != 0.0f;
}

//移動速度と回転速度を制限した移動入力
bool Player::input_move(float elapsedTime, Camera* camera, float restriction_move, float restriction_turn)
{
	//進行ベクトル取得
	const DirectX::XMFLOAT3 move_vec = get_move_vec(camera);

	//移動処理
	Move(move_vec.x, move_vec.z, this->move_speed / restriction_move);
	Turn(elapsedTime, move_vec, this->turn_speed / restriction_turn, orientation);

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
			Jump(jump_speed);
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
void Player::input_chant_support_skill(Graphics& graphics)
{
	DirectX::XMFLOAT3 launch_pos;
	if (game_pad->get_button() & GamePad::BTN_LEFT_TRIGGER) //左トリガーでサポートスキル発動
	{
		switch (skill_manager->get_selected_sup_skill_type())
		{
		case SP_SKILLTYPE::PHYSICAL_UP:
			if (skill_manager->chant_physical_up(graphics, position, &move_speed, &jump_speed))
			{
				transition_magic_buff_state();//状態遷移
			}
			break;
			case SP_SKILLTYPE::REGENERATE:
				//if (skill_manager->chant_regenerate(graphics, launch_pos, ))
				//{
				//	transition_magic_buff_state();//状態遷移
				//};

			break;
			case SP_SKILLTYPE::RESTRAINNT:
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
void Player::input_chant_attack_skill(Graphics& graphics)
{
	DirectX::XMFLOAT3 launch_pos;
	if (game_pad->get_button() & GamePad::BTN_RIGHT_TRIGGER)  //右トリガーで攻撃スキル発動
	{
		switch (skill_manager->get_selected_atk_skill_type())
		{
		case ATK_SKILLTYPE::MAGICBULLET :
			model->fech_by_bone(transform, left_hand, launch_pos);
			//スキルを発動できた場合遷移
			if (skill_manager->chant_magic_bullet(graphics, launch_pos, Math::get_posture_forward(orientation)))
			{
				transition_attack_bullet_state();//状態遷移
			}
			break;
		case ATK_SKILLTYPE::SPEARS_SEA:
			if (skill_manager->chant_spear_sea(graphics, position))
			{
			   transition_attack_ground_state();
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
	skill_manager->judge_magic_bullet_vs_enemy(object_colider, damaged_func);
	skill_manager->judge_spear_sea_vs_enemy(object_colider, damaged_func);
}
//==============================================================
// 
//自分と敵の体の当たり判定処理
// 
//==============================================================
void Player::calc_collision_vs_enemy(Capsule collider, float collider_height)
{
	Collision::cylinder_vs_cylinder(collider.start, collider.radius, collider_height, position, radius, height, &position);
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
		if (Collision::capsule_vs_capsule(collider.start, collider.end, collider.radius, attack_sword_param.collision.start, attack_sword_param.collision.end, attack_sword_param.collision.radius))
		{
			//攻撃対象に与えるダメージ量と無敵時間
			damaged_func(attack_sword_param.power, attack_sword_param.invinsible_time, WINCE_TYPE::NONE);
			//ヒットエフェクト再生
			if (!test_slash_hit->get_active())
			{
				//ヒットエフェクト
				test_slash_hit->play({ collider.start.x, collider.start.y +20, collider.start.z});
				test_slash_hit->set_life_span(0.1f);
				test_slash_hit->set_rotate_quaternion(MeshEffect::AXIS::UP, Noise::instance().random_range(0, 90));
				test_slash_hit->set_rotate_quaternion(MeshEffect::AXIS::FORWARD, Noise::instance().random_range(0, 90));
				//ヒットストップ
				camera->set_camera_stop(0.1f);
			}
			//test_slash_hit->play(attack_sword_param.collision.end,100.0f);
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
		velocity.y /= floating_value;
		return true;
	}
	//浮遊中でない
	return false;
}

//==============================================================
// 
//デバッグGUI表示
// 
//==============================================================
void Player::debug_gui(Graphics& graphics)
{
#ifdef USE_IMGUI
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	imgui_menu_bar("Charactor", "player", display_player_imgui);
	if (display_player_imgui)
	{
		if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
		{
			ImGui::DragFloat("add_root_speed", &add_root_speed);
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
				const char* state_c[] = { "IDLE","MOVE","JUMP","FALL","LANDING"};

				ImGui::Text(state_c[static_cast<int>(state)]);
				ImGui::DragFloat3("velocity:", &velocity.x);
			}
			if (ImGui::CollapsingHeader("Param", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::DragFloat("height", &height);
				ImGui::DragInt("health", &health);
				ImGui::DragFloat("radius", &radius);
				ImGui::DragFloat("gravity", &gravity);
				ImGui::DragFloat("floating_value", &floating_value);
				ImGui::DragFloat("invinsible_timer", &invincible_timer);
				ImGui::DragFloat("MoveSpeed", &move_speed);
				ImGui::DragFloat("avoidance_speed", &avoidance_speed);
				ImGui::DragFloat("friction", &friction);
				ImGui::DragFloat("acceleration", &acceleration);
				ImGui::DragFloat("jump_speed", &jump_speed);
				ImGui::DragFloat("air_control", &air_control);
				ImGui::Checkbox("is_ground", &is_ground);
				float control_x = game_pad->get_axis_LX();
				float control_y = game_pad->get_axis_LY();
				ImGui::DragFloat("control_x", &control_x);
				ImGui::DragFloat("control_y", &control_y);
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
				ImGui::DragFloat("playback_speed", &model->anime_param.playback_speed,0.1f);
				ImGui::DragFloat("blend_time", &model->anime_param.blend_time);
				ImGui::DragFloat("sampling", &model->anime_param.animation.sampling_rate);

			}
			

			ImGui::Begin("Skill");
			
				//--------スキルのデバッグGUI--------//
				if (ImGui::Button("sup_skill_chant"))
				{
					switch (skill_manager->get_selected_sup_skill_type())
					{
					case SP_SKILLTYPE::PHYSICAL_UP:
						skill_manager->chant_physical_up(graphics, position, &skill_add_move_speed, &skill_add_jump_speed);
						break;
					case SP_SKILLTYPE::REGENERATE:
						transition_magic_buff_state();//状態遷移

						break;
					case SP_SKILLTYPE::RESTRAINNT:
						transition_attack_pull_slash_state();
						break;
					default:
						break;
					}
				}
				ImGui::SameLine();
				DirectX::XMFLOAT3 launch_pos;
				if (ImGui::Button("atk_skill_chant"))
				{
					switch (skill_manager->get_selected_atk_skill_type())
					{
					case ATK_SKILLTYPE::MAGICBULLET:
						model->fech_by_bone(transform, left_hand, launch_pos);

						if (skill_manager->chant_magic_bullet(graphics, launch_pos, Math::get_posture_forward(orientation)))
						{
							transition_attack_bullet_state();//状態遷移
						}
						break;
					case ATK_SKILLTYPE::SPEARS_SEA:
						if (skill_manager->chant_spear_sea(graphics, position))
						{
							transition_attack_ground_state();
						}
						break;
					default:
						break;
					}
				}
				ImGui::End();
		}
		ImGui::End();

	}



	skill_manager.get()->debug_gui(graphics);
	slash_efect->debug_gui("slash_efect");
	test_slash_hit->debug_gui("test_slash_hit");
#endif // USE_IMGUI

}

