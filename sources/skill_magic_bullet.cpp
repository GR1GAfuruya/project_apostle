#include "skill_magic_bullet.h"
#include "Operators.h"

 MagicBullet::MagicBullet(Graphics& graphics, DirectX::XMFLOAT3 init_pos, DirectX::XMFLOAT3 target_pos)
{
	 initialize(graphics);
	 position = init_pos;
	 velocity = Math::calc_vector_AtoB_normalize(target_pos, position);
}

void MagicBullet::initialize(Graphics& graphics)
{
	life_time = 5;
	cool_time = 0.2f;
	collision_type = CollisionType::SPHERE;
	power = 3.0f;
	invinsible_time = 0.1f;
	attack_colider.radius = 2;
	
	
}

void MagicBullet::update(Graphics& graphics, float elapsed_time)
{
	life_time -= elapsed_time;
	if (life_time < 0) skill_end_flag = true;
	velocity *= acceleration;

	position += velocity;
	attack_colider.start = position;

}

void MagicBullet::render(Graphics& graphics)
{

}

void MagicBullet::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "MagicBullet:" + str_id + "Param";
	ImGui::Begin("Skill");
	ImGui::PushID(str_id.c_str());
	/*これより下にパラメーター記述*/
	ImGui::BulletText(name.c_str());
	ImGui::DragFloat("life_time", &life_time);
	ImGui::DragFloat("acceleration", &acceleration);
	ImGui::DragFloat("power", &power);
	ImGui::DragFloat("invinsible_time", &invinsible_time);
	

	/*これより上にパラメーター記述*/
	ImGui::PopID();
	ImGui::End();

#endif

}
