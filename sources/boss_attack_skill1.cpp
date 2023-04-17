#include "boss_attack_skill1.h"
#include "stage_manager.h"
//==============================================================
// 
//コンストラクタ
// 
//==============================================================
BossAttackSkill1::BossAttackSkill1(Graphics& graphics)
{
	const DirectX::XMFLOAT4 FIRE_COLOR = { 4.0f, 1.0f, 0.7f, 0.8f };

	//腕のエフェクト
	{
		arm_effect = make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_tornado4.fbx");
		arm_effect->set_material(MaterialManager::instance().mat_fire_distortion.get());
		arm_effect->set_init_color(FIRE_COLOR);
		arm_effect->set_init_scale(0);
		arm_effect->set_init_life_duration(0.5f);
	}
	//爆発後の余韻エフェクト
	wave_effect = make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/torus.fbx");
	wave_effect->set_material(MaterialManager::instance().mat_fire_distortion.get());
	wave_effect->set_init_color({ 4.0f, 1.0f, 0.7f, 0.8f });
	wave_effect->set_init_life_duration(2.0f);
	wave_effect->set_init_scale(0);

	//更新関数初期化
	state_update = [=](Graphics& graphics, float elapsed_time, Camera* camera)
		->void {return attack_state_update(graphics, elapsed_time, camera); };

	wave_params.position = { 0,0,0 };
	wave_params.height = 2;
	wave_params.width = 5;
	wave_params.radius = 0;
	wave_params.max_radius = 80;
	wave_params.expansion_speed = 240.0f;
	wave_params.is_hit = false;
	wave_effect->set_scale(0);
	wave_effect->set_position({ 0,0,0 });
	wave_effect->set_dissolve_flag(false);
	//カメラシェイク
	{
		camera_shake.max_x_shake = 5.0f;
		camera_shake.max_y_shake = 10.0f;
		camera_shake.time = 0.5f;

	}
	at_param.power = 100;
	at_param.invinsible_time = 2.0f;
	range = 10;

	wave_effect->play(arm_pos);
	wave_effect->set_is_loop(true);
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
	boss_position = pos;
	wave_params.is_hit = false;
	wave_params.active = true;
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

	wave_effect->set_scale({ wave_params.radius /25.0f ,wave_params.height / 10.0f,wave_params.radius / 25.0f });
	//wave_params.width = wave_params.radius / 5.0f;
	wave_effect->set_position(wave_params.position);
	wave_effect->update(graphics, elapsed_time);
	arm_effect->update(graphics, elapsed_time);
	//meteore_effect->update(graphics, elapsed_time);

	if (wave_params.active )
	{
		state_update(graphics, elapsed_time, camera);
	}
};



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
		//カメラシェイク
		camera->set_camera_shake(camera_shake);
		//腕エフェクトストップ
		arm_effect->stop();
		//タイマーリセット
		charge_timer = 0;

		wave_effect->play(arm_pos);
		wave_effect->set_is_loop(true);
		wave_params.radius = 0;
		/*wave_effect->set_scale({0,wave_params.height,0});
		wave_effect->set_position({ 0,0,0 });*/

	}

	//エフェクトの更新
	{
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
	wave_params.position = boss_position;

	//リングが最大値まで達したら終了
	if (wave_params.radius > wave_params.max_radius)
	{
		wave_params.active = false;
		wave_params.radius = 0;
	}
	else
	{
		wave_params.radius += wave_params.expansion_speed * elapsed_time;
	}

}

//==============================================================
// 
//描画
// 
//==============================================================
void BossAttackSkill1::render(Graphics& graphics,Camera* camera)
{
	wave_effect->render(graphics);
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
	imgui_menu_bar("Effects", "boss_attack_skill1", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("attack_skill1");
		ImGui::DragFloat3("position", &wave_params.position.x, 0.1f);
		ImGui::DragFloat("wave_params.height", &wave_params.height, 0.1f, 0);
		ImGui::DragFloat("wave_params.width", &wave_params.width, 0.1f, 0);
		ImGui::DragFloat("wave_params.radius", &wave_params.radius, 1, 0);
		ImGui::DragFloat("wave_params.expansion_speed", &wave_params.expansion_speed, 1, 0);
		ImGui::DragFloat("wave_params.max_radius", &wave_params.max_radius, 1, 0);
		if (ImGui::CollapsingHeader("camera_shake"))
		{
			ImGui::DragFloat("combo1_shake_x", &camera_shake.max_x_shake, 0.1f);
			ImGui::DragFloat("combo1_shake_y", &camera_shake.max_y_shake, 0.1f);
			ImGui::DragFloat("combo1_time", &camera_shake.time, 0.1f);
			ImGui::DragFloat("combo1_smmoth", &camera_shake.shake_smoothness, 0.1f, 0.1f, 1.0f);
		}
		ImGui::End();
	}

#endif //  USE_IMGUI

}

void BossAttackSkill1::calc_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func)
{
	if (Collision::ring_vs_capsule(wave_params.position, wave_params.radius, wave_params.width, wave_params.height, capsule_start, capsule_end, colider_radius))
	{
		int a = 0;
		//ダメージを与える
		damaged_func(at_param.power, at_param.invinsible_time, WINCE_TYPE::SMALL);
		on_hit();
	}

};


void BossAttackSkill1::on_hit()
{
	wave_params.is_hit = true;
}

