#include "boss_attack_skill1.h"
#include "stage_manager.h"
#include "user.h"
//==============================================================
// 
//コンストラクタ
// 
//==============================================================
BossAttackSkill1::BossAttackSkill1(Graphics& graphics)
{
	const DirectX::XMFLOAT4 FIRE_COLOR = { 4.0f, 1.0f, 0.7f, 0.8f };
	//スキル時間
	skill_time = 1.0f;


	//腕のエフェクト
	{
		arm_effect = make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_tornado4.fbx");
		arm_effect->set_material(MaterialManager::instance().mat_fire_distortion.get());
		arm_effect->set_init_color(FIRE_COLOR);
		arm_effect->set_init_scale(0);
		arm_effect->set_init_life_duration(0.5f);
	}
	//波動エフェクト
	wave = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_aura.fbx");
	wave->set_material(MaterialManager::instance().mat_fire_distortion.get());
	wave->set_init_life_duration(skill_time);
	wave->set_init_scale({ 0.0f, 0.0f, 11.0f });
	wave->set_init_color({ FIRE_COLOR });

	params.colider_ring.height = 2.0f;
	params.colider_ring.width = 20.0f;
	params.colider_ring.radius = 0.0f;
	params.scale_speed = 5.0f;

	//更新関数初期化
	state_update = [=](Graphics& graphics, float elapsed_time, Camera* camera)
		->void {return none_update(graphics, elapsed_time, camera); };


	//カメラシェイク
	{
		camera_shake.max_x_shake = 5.0f;
		camera_shake.max_y_shake = 10.0f;
		camera_shake.time = 0.5f;

	}

	//TODO:メテオの速度などのパラメーター　※のちにJSON化！！！
	at_param.power = 100;
	at_param.invinsible_time = 2.0f;
	range = 10;


	const float CHARGE_TIME = 2.0f;
	charge_time = CHARGE_TIME;
}

BossAttackSkill1::~BossAttackSkill1()
{
}

//==============================================================
// 
//発動
// 
//==============================================================
void BossAttackSkill1::chant(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir)
{
	state_update = [=](Graphics& graphics, float elapsed_time, Camera* camera)
		->void {return charge_state_update(graphics, elapsed_time, camera); };

	arm_effect->play(arm_pos);
	arm_effect->rotate_base_axis(MeshEffect::AXIS::UP, arm_dir);

}

//==============================================================
// 
//停止
// 
//==============================================================
void BossAttackSkill1::stop()
{
}
//==============================================================
// 
//更新
// 
//==============================================================
void BossAttackSkill1::update(Graphics& graphics, float elapsed_time, Camera* camera, DirectX::XMFLOAT3 arm_pos, DirectX::XMFLOAT3 arm_dir)
{
	this->arm_pos = arm_pos;
	this->arm_dir = arm_dir;

	state_update(graphics, elapsed_time, camera);

	arm_effect->update(graphics, elapsed_time);
	wave->update(graphics, elapsed_time);
};

//==============================================================
// 
//何もしていない時の更新
// 
//==============================================================

void BossAttackSkill1::none_update(Graphics& graphics, float elapsed_time, Camera* camera)
{
	params.colider_ring.radius = 0.0f;
	wave->set_scale({ 0, 0, wave->get_scale().z });

}



//==============================================================
// 
//チャージしているときの更新
// 
//==============================================================
void BossAttackSkill1::charge_state_update(Graphics& graphics, float elapsed_time, Camera* camera)
{
	charge_timer += elapsed_time;
	if (charge_timer > charge_time)
	{
		//更新関数を攻撃に
		state_update = [=](Graphics& graphics, float elapsed_time, Camera* camera)
			->void {return attack_state_update(graphics, elapsed_time, camera); };
		//メテオの位置設定
		params.colider_ring.center = arm_pos;
		//カメラシェイク
		camera->set_camera_shake(camera_shake);
		//腕エフェクトストップ
		arm_effect->stop();
		//タイマーリセット
		charge_timer = 0;
	}

	//エフェクトの更新
	{
		arm_effect->set_position(arm_pos);
		arm_effect->set_position(arm_pos);
		arm_effect->rot_speed.y = 180.0f * elapsed_time;
		arm_effect->rotate_base_axis(MeshEffect::AXIS::UP, arm_dir);

		const float scale_speed = 10.0f;
		arm_effect->set_scale(arm_effect->get_scale().x + scale_speed * elapsed_time );

		if (!arm_effect->get_active())
		{
			arm_effect->play(arm_pos);
		}
	}
}

//==============================================================
// 
//攻撃したときの更新
// 
//==============================================================
void BossAttackSkill1::attack_state_update(Graphics& graphics, float elapsed_time, Camera* camera)
{
	if (!wave->get_active())
	{
		wave->play(arm_pos);
		
	}
	//拡大を繰り返して波形っぽく
	const float omen_add_scale = wave->get_scale().x + params.scale_speed * elapsed_time;
	wave->set_scale({ omen_add_scale, omen_add_scale, wave->get_scale().z });
	wave->rotate_base_axis(MeshEffect::AXIS::FORWARD, { 0,1,0 });

	params.colider_ring.center = arm_pos;
	params.colider_ring.radius += params.scale_speed * elapsed_time;

	if (wave->get_life_time() >= skill_time)
	{
		state_update = [=](Graphics& graphics, float elapsed_time, Camera* camera)
			->void {return none_update(graphics, elapsed_time, camera); };

	}
}

//==============================================================
// 
//描画
// 
//==============================================================
void BossAttackSkill1::render(Graphics& graphics,Camera* camera)
{
	wave->render(graphics);
	arm_effect->render(graphics,camera);
}

//==============================================================
// 
//デバッグGUI
// 
//==============================================================
void BossAttackSkill1::debug_gui(const char* str_id)
{
#if  USE_IMGUI
	wave->debug_gui("skill1_wave");

	imgui_menu_bar("Effects", "boss_skill1", display_imgui); 
	if (display_imgui)
	{
		ImGui::Begin("boss_skill1_");
		ImGui::DragFloat("center", &params.colider_ring.center.x,0.1f);
		ImGui::DragFloat("radius", &params.colider_ring.radius,0.1f);
		ImGui::DragFloat("width", &params.colider_ring.width,0.1f);
		ImGui::DragFloat("height", &params.colider_ring.height,0.1f);
		ImGui::DragFloat("scale_speed", &params.scale_speed,0.1f);
		ImGui::DragFloat("skill_time", &skill_time,0.1f);
		ImGui::End();
	}
#endif //  USE_IMGUI

}

void BossAttackSkill1::calc_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func)
{
		if (!params.is_hit)
		{
			if (Collision::ring_vs_capsule(params.colider_ring.center, params.colider_ring.width, params.colider_ring.height, params.colider_ring.radius,
				capsule_start, capsule_end, colider_radius))
			{
				//当たり判定の位置と大きさ更新
				damaged_func(at_param.power, at_param.invinsible_time, WINCE_TYPE::SMALL);
				params.is_hit = true;
			}
		}
	
}


