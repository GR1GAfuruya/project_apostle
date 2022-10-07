#include "skill_magic_bullet.h"
#include "Operators.h"

 MagicBullet::MagicBullet(Graphics& graphics, DirectX::XMFLOAT3 init_pos, DirectX::XMFLOAT3 dir, InitializeParam init_param)
{
	 initialize(graphics);
	

	 power = init_param.power;
	 invinsible_time = init_param.invisible_time;
	 acceleration = init_param.acceleration;
	 attack_colider.radius = init_param.collider_radius;
	 cool_time = init_param.cool_time;

	 position = init_pos;
	 velocity = acceleration * Math::Normalize(dir);
}

void MagicBullet::initialize(Graphics& graphics)
{
	life_time = 5;
	collision_type = CollisionType::SPHERE;
	
}

void MagicBullet::update(Graphics& graphics, float elapsed_time)
{
	
	
	position += velocity * elapsed_time;
	attack_colider.start = position;


	//消滅処理
	life_time -= elapsed_time;
	if(is_hit) skill_end_flag = true;
	if (life_time < 0) skill_end_flag = true;
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
	ImGui::DragFloat3("position", &position.x);
	ImGui::DragFloat3("velocity", &velocity.x);
	ImGui::DragFloat("life_time", &life_time);
	

	/*これより上にパラメーター記述*/
	ImGui::PopID();
	ImGui::End();

#endif

}
