#include "boss.h"
#include "user.h"
Boss::Boss(Graphics& graphics)
{
	model = make_unique<SkeletalMesh>(graphics.get_device().Get(), "./resources/Model/Boss/LordHell.fbx", 60);
	efc_charge_attack = make_unique<ChargeAttack>(graphics);
	scale.x = scale.y = scale.z = 0.1f;
	transition_idle_state();
	efc_charge_attack->play({ position.x,position.y + 50,position.z });
	
}

void Boss::initialize()
{
}

void Boss::update(Graphics& graphics, float elapsed_time, Stage* stage)
{

	model->update_animation(elapsed_time);
	update_velocity(elapsed_time, position, stage);
	efc_charge_attack->update(graphics, elapsed_time);
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
}

