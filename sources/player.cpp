﻿
#include"player.h"
#include"camera.h"
#include "shader.h"
#include"user.h"
#include "texture.h"
#include "Operators.h"
#include "../external/magic_enum/include/magic_enum.hpp"
void Player::initialize()
{
	//パラメーター初期化
	position = { 46.0f, 0.0f,250.0f };
	velocity = { 0.0f, 0.0f, 0.0f };
	jump_count = 1;
	
}

Player::Player(Graphics& graphics, Camera* camera)
{
	model = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), "./resources/Model/Player/womanParadin.fbx", 30.0f);
	slash_efect = std::make_unique<Slash>(graphics.get_device().Get());
	slash_efect->set_scale(0.15f);
	slash_efect->constants->data.particle_color = { 1.8f,1.8f,5.2f,0.8f };
	model->play_animation(PlayerAnimation::PLAYER_IDLE, true);
	state = State::IDLE;
	scale.x = scale.y = scale.z = 0.05f;
	radius = 1.0f;
	height = 5.5f;
	jump_speed = 27.0f;
	attack1 = std::make_unique<GPU_Particles>(graphics.get_device().Get());
	attack1.get()->initialize(graphics.get_dc().Get());
	attack1->particle_constants->data.particle_color = { 1.0f,0.8f,8.5f,0.7f };
	mouse = &Device::instance().get_mouse();
	game_pad = &Device::instance().get_game_pad();

	sword_hand = model->get_bone_by_name("pelvis");
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

	model->update_animation(elapsed_time);
	//デバッグGUI描画
	debug_gui();
}

//描画処理
void Player::render_d(Graphics& graphics, float elapsed_time, Camera* camera)
{
	// 拡大縮小（S）・回転（R）・平行移動（T）行列を計算する
	// スタティックメッシュ
	world = Math::calc_world_matrix(scale, orientation, position);
	graphics.shader->render(graphics.get_dc().Get(), model.get(), world);
}

void Player::render_f(Graphics& graphics, float elapsed_time, Camera* camera)
{
	slash_efect->render(graphics);
	attack1->render(graphics.get_dc().Get(),graphics.get_device().Get());
}



const DirectX::XMFLOAT3 Player::get_move_vec(Camera* camera) const
{
	GamePad& game_pad = Device::instance().get_game_pad();
	//入力情報を取得
	float ax = game_pad.get_axis_LX();
	float ay = game_pad.get_axis_LY();

	//移動ベクトルはXZ平面に水平なベクトルになるようにする

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
	DirectX::XMFLOAT3 emit_pos = position + Math::vector_scale(get_posture_forward_vec(orientation),14);
	emit_pos.y = position.y + 3.0f;
	//attack1.get()->particle_constants.get()->data.particle_size = { 0.1,0.6 };
	//attack1.get()->particle_constants.get()->data.emitter.emit_rate = 1200.0f;
	attack1.get()->set_emitter_pos(emit_pos);
	attack1.get()->set_emitter_rate(1500);
	attack1.get()->set_emitter_life_time(1);
	attack1.get()->launch_emitter( emit_cs);
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
	if (game_pad->get_button_down() & game_pad->BTN_A) //スペースを押したらジャンプ
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



void Player::debug_gui()
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
				DirectX::XMFLOAT3 a{};
				a.x = DirectX::XMConvertToDegrees(angle.x);
				a.y = DirectX::XMConvertToDegrees(angle.y);
				a.z = DirectX::XMConvertToDegrees(angle.z);
				ImGui::DragFloat3("Angle", &a.x);
				angle.x = DirectX::XMConvertToRadians(a.x);
				angle.y = DirectX::XMConvertToRadians(a.y);
				angle.z = DirectX::XMConvertToRadians(a.z);
				DirectX::XMFLOAT3 forward;
				DirectX::XMStoreFloat3(&forward, get_posture_forward_vec(orientation));
				ImGui::DragFloat3("forward", &forward.x);
				ImGui::DragFloat4("ori", &orientation.x);
				const char* state_c[] = { "IDLE","MOVE","JUMP","FALL","LANDING"};

				ImGui::Text(state_c[static_cast<int>(state)]);
				ImGui::DragFloat3("velocity:", &velocity.x);
			}
			if (ImGui::CollapsingHeader("Param", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::DragFloat("height", &height);
				ImGui::DragFloat("radius", &radius);
				ImGui::DragFloat("gravity", &gravity);
				ImGui::DragFloat("invinsible_timer", &invinsible_timer);
				ImGui::DragFloat("maxMoveSpeed", &move_speed);
				ImGui::DragFloat("friction", &friction);
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
				ImGui::DragFloat("blend_time", &model->anime_param.blend_time);
				ImGui::DragFloat("sampling", &model->anime_param.animation.sampling_rate);

			}
		}
		ImGui::End();
	}
#endif // USE_IMGUI
}




