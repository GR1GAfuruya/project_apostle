#include "boss.h"
#include "user.h"
Boss::Boss(Graphics& graphics)
{
	model = make_unique<SkeletalMesh>(graphics.get_device().Get(), "./resources/Model/Boss/LordHell.fbx", 60.0f);
	efc_charge_attack = make_unique<ChargeAttack>(graphics);
	
	initialize();

}

void Boss::initialize()
{
	transition_idle_state();
	scale.x = scale.y = scale.z = 0.1f;
	health = 1000;
	velocity = { 0.0f, 0.0f, 0.0f };
	efc_charge_attack->stop();
	attack_power = 10;
	attack_add_invisible_time = 0.5f;
	damaged_function = [=](int damage, float invincible)->void {apply_damage(damage, invincible); };
}

void Boss::update(Graphics& graphics, float elapsed_time, Stage* stage)
{
	(this->*act_update)(graphics, elapsed_time, stage);
	model->update_animation(elapsed_time);
	update_velocity(elapsed_time, position, stage);
	efc_charge_attack->update(graphics, elapsed_time);
	boss_collision.position = position;
	boss_collision.radius = 10;
	boss_collision.height = 25;
	boss_collision.position_end = boss_collision.position;
	boss_collision.position_end.y = boss_collision.position.y + boss_collision.height;

	update_invicible_timer(elapsed_time);
	debug_gui();
}

void Boss::render_d(Graphics& graphics, float elapsed_time)
{
	world = Math::calc_world_matrix(scale, orientation, position);
	graphics.shader->render(graphics.get_dc().Get(), model.get(), world);
}

void Boss::render_f(Graphics& graphics, float elapsed_time)
{
	efc_charge_attack->render(graphics);
	
}

void Boss::debug_gui()
{

#if USE_IMGUI
	imgui_menu_bar("charactor", "boss", display_imgui);
	if (display_imgui)
	{
		if (ImGui::Begin("Boss", nullptr, ImGuiWindowFlags_None))
		{
			if (ImGui::Button("charge_attack")) transition_skill_2_start_state();
			ImGui::DragInt("hp", &health);
			ImGui::DragFloat("height", &height);
		}
		ImGui::End();
	}
	efc_charge_attack->debug_gui("");
#endif
}

void Boss::calc_attack_vs_player(DirectX::XMFLOAT3 player_cap_start, DirectX::XMFLOAT3 player_cap_end, float colider_radius, AddDamageFunc damaged_func)
{
	DirectX::XMFLOAT3 boss_collider_end = position;
	boss_collider_end.y = position.y + height;
	if (Collision::capsule_vs_capsule(player_cap_start, player_cap_end, colider_radius, position, boss_collider_end, 10.0f))
	{
		damaged_func(attack_power, attack_add_invisible_time);
	}
}