
#include"player.h"
#include"camera.h"
#include "shader.h"
#include"user.h"
#include "texture.h"
#include "Operators.h"
#include "../external/magic_enum/include/magic_enum.hpp"
#include "collision.h"
void Player::initialize()
{
	//パラメーター初期化
	position = { 46.0f, 10.0f,250.0f };
	velocity = { 0.0f, 0.0f, 0.0f };
	move_speed = 30.0f;
	turn_speed = DirectX::XMConvertToRadians(720);
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
	damaged_function = [=](int damage, float invincible)->void {apply_damage(damage, invincible); };
	state = State::IDLE;
	attack1->particle_constants->data.particle_color = { 1.0f,0.8f,8.5f,0.7f };
	sword->initialize();

}

Player::Player(Graphics& graphics, Camera* camera)
{
	//キャラクターモデル
	model = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), "./resources/Model/Player/womanParadin.fbx", 30.0f);
	skill_manager = std::make_unique<SkillManager>(graphics);
	//キャラが持つ剣
	sword = std::make_unique<Sword>(graphics);
	//攻撃時エフェクト
	slash_efect = std::make_unique<Slash>(graphics.get_device().Get());
	slash_efect->set_scale(0.15f);
	slash_efect->constants->data.particle_color = { 1.8f,1.8f,5.2f,0.8f };

	
	attack1 = std::make_unique<GPU_Particles>(graphics.get_device().Get(),200000);
	attack1.get()->initialize(graphics.get_dc().Get());
	mouse = &Device::instance().get_mouse();
	game_pad = &Device::instance().get_game_pad();

	sword_hand = model->get_bone_by_name("pelvis");
	right_hand = model->get_bone_by_name("hand_r");
	create_cs_from_cso(graphics.get_device().Get(), "shaders/boss_attack1_emit_cs.cso", emit_cs.ReleaseAndGetAddressOf());
	create_cs_from_cso(graphics.get_device().Get(), "shaders/boss_attack1_update_cs.cso", update_cs.ReleaseAndGetAddressOf());
	initialize();
}


//デストラクタ
Player::~Player()
{
}



//更新処理
void Player::update(Graphics& graphics, float elapsed_time, Camera* camera,Stage* stage)
{
	//更新処理
	(this->*p_update)(graphics, elapsed_time, camera,stage);
	
	//オブジェクト行列を更新
	//無敵時間の更新
	update_invicible_timer(elapsed_time);
	slash_efect->update(graphics,elapsed_time);
	attack1.get()->update(graphics.get_dc().Get(),elapsed_time, update_cs.Get());
	skill_manager.get()->update(graphics, elapsed_time);
	model->update_animation(elapsed_time);
	DirectX::XMFLOAT4X4 sword_hand_mat = {};
	model->fech_bone_world_matrix(transform, right_hand, &sword_hand_mat);


	//ソード更新
	{
		sword->set_parent_transform(sword_hand_mat);
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
	select_support_skill();
	input_chant_support_skill(graphics);
	input_chant_attack_skill(graphics);
	//スキル選択中カメラ操作ストップ
	camera->set_camera_operate_stop(skill_manager.get()->is_selecting_skill());
	if (skill_manager.get()->is_selecting_skill())
	{
		int a = 1;
	}
}

//描画処理
void Player::render_d(Graphics& graphics, float elapsed_time, Camera* camera)
{
	// 拡大縮小（S）・回転（R）・平行移動（T）行列を計算する
	// スタティックメッシュ
	transform = Math::calc_world_matrix(scale, orientation, position);
	graphics.shader->render(graphics.get_dc().Get(), model.get(), transform);
	//剣描画
	sword->render(graphics);
}

void Player::render_f(Graphics& graphics, float elapsed_time, Camera* camera)
{
	slash_efect->render(graphics);
	attack1->render(graphics.get_dc().Get(),graphics.get_device().Get());
	skill_manager.get()->render(graphics);
	attack1->debug_gui("player_attack1");
	//デバッグGUI描画
	debug_gui(graphics);

}

void Player::render_ui(Graphics& graphics, float elapsed_time)
{
	skill_manager.get()->ui_render(graphics, elapsed_time);
}



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


void Player::Attack(Graphics& graphics, float elapsed_time)
{
	DirectX::XMFLOAT3 emit_pos = position + Math::vector_scale(Math::get_posture_forward_vec(orientation),14);
	emit_pos.y = position.y + 3.0f;
	//attack1.get()->particle_constants.get()->data.particle_size = { 0.1,0.6 };
	//attack1.get()->particle_constants.get()->data.emitter.emit_rate = 1200.0f;
	attack1.get()->set_emitter_pos(emit_pos);
	attack1.get()->set_emitter_rate(150);
	attack1.get()->set_particle_size({0.1f,0.1f});
	attack1.get()->set_emitter_life_time(0.2f);
	attack1.get()->launch_emitter( emit_cs);
}

void Player::select_support_skill()
{
	if (game_pad->get_button_down() & GamePad::BTN_LEFT_SHOULDER) //左トリガーを引いたら支援スキル
	{
		//transition_support_magic_state();
	}
}

bool Player::input_move(float elapsedTime, Camera* camera)
{
	//進行ベクトル取得
	const DirectX::XMFLOAT3 move_vec = get_move_vec(camera);

	//移動処理
	Move(move_vec.x, move_vec.z, this->move_speed);
	Turn(elapsedTime, move_vec, this->turn_speed/2.0f, orientation);

	return move_vec.x != 0.0f || move_vec.y != 0.0f || move_vec.z != 0.0f;
}


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

void Player::input_avoidance()
{
	if (game_pad->get_button_down() & GamePad::BTN_B)
	{
		transition_avoidance_state();
	}
}

//サポートスキル発動
void Player::input_chant_support_skill(Graphics& graphics)
{
	if (game_pad->get_button() & GamePad::BTN_LEFT_TRIGGER) //左トリガーでサポートスキル発動
	{
		switch (skill_manager->get_selected_sup_skill_type())
		{
		case SP_SKILLTYPE::PHYCICAL_UP:
		//	skill_manager->chant_phycical_up(graphics, position, position);
			break;
			case SP_SKILLTYPE::REGENERATE:

			break;
			case SP_SKILLTYPE::RESTRAINNT:

			break;
		default:
			break;
		}
		transition_support_magic_state();//状態遷移
	}
}

//攻撃スキル発動
void Player::input_chant_attack_skill(Graphics& graphics)
{
	if (game_pad->get_button() & GamePad::BTN_RIGHT_TRIGGER)  //右トリガーで攻撃スキル発動
	{
		switch (skill_manager->get_selected_atk_skill_type())
		{
		case ATK_SKILLTYPE::MAGICBULLET :
			skill_manager->chant_magic_bullet(graphics, position, Math::get_posture_forward(orientation));
			break;
		case ATK_SKILLTYPE::SPEARS_SEA:
			skill_manager->chant_spear_sea(graphics, position);
			break;
		default:
			break;
		}
		
		transition_attack_bullet_state();//状態遷移
	}
}

void Player::judge_skill_collision(Capsule object_colider, AddDamageFunc damaged_func)
{
	skill_manager->judge_magic_bullet_vs_enemy(object_colider, damaged_func);
}

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

void Player::on_dead()
{
	initialize();
}

void Player::on_damaged()
{
	transition_damage_front_state();	
}


void Player::debug_gui(Graphics& graphics)
{
#ifdef USE_IMGUI
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	imgui_menu_bar("charactor", "player", display_player_imgui);
	if (display_player_imgui)
	{
		if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
		{
			
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
				ImGui::DragFloat("invinsible_timer", &invincible_timer);
				ImGui::DragFloat("maxMoveSpeed", &move_speed);
				ImGui::DragFloat("avoidance_speed", &avoidance_speed);
				ImGui::DragFloat("friction", &friction);
				ImGui::DragFloat("acceleration", &acceleration);
				ImGui::DragFloat("jump_speed", &jump_speed);
				ImGui::DragFloat("air_control", &air_control);
				ImGui::Checkbox("is_ground", &is_ground);
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
			float control_x = game_pad->get_axis_LX();
			float control_y = game_pad->get_axis_LY();
			ImGui::DragFloat("control_x", &control_x);
			ImGui::DragFloat("control_y", &control_y);
		}
		ImGui::End();

		ImGui::Begin("Skill");
		{
			ImGui::Text("player_skill_system");
			
			
		}
		ImGui::End();
		skill_manager.get()->debug_gui(graphics);

	}
#endif // USE_IMGUI

}

void Player::calc_collision_vs_enemy(Capsule collider,float collider_height)
{
	Collision::cylinder_vs_cylinder(collider.start, collider.radius, collider_height, position, radius, height, &position);
}

void Player::calc_attack_vs_enemy(Capsule collider, AddDamageFunc damaged_func)
{
	//剣の攻撃中のみ当たり判定
	if (attack_sword_param.is_attack)
	{
		if (Collision::capsule_vs_capsule(collider.start, collider.end, collider.radius, attack_sword_param.collision.start, attack_sword_param.collision.end, attack_sword_param.collision.radius))
		{
			//攻撃対象に与えるダメージ量と無敵時間
			damaged_func(attack_sword_param.power, attack_sword_param.invinsible_time);
		}
	}
}

