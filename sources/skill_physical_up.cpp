#include "skill_physical_up.h"

PhysicalUp::PhysicalUp(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, float* run_speed, float* jump_speed)
{
	*run_speed *= magnification;
	*jump_speed *= magnification;
	enhanced_run_speed.reset(run_speed);
	enhanced_jump_speed.reset(jump_speed);
	initialize(graphics);
}

PhysicalUp::~PhysicalUp()
{
	*enhanced_run_speed /= magnification;
	*enhanced_jump_speed /= magnification;
	enhanced_run_speed.release();
	enhanced_jump_speed.release();


}

void PhysicalUp::initialize(Graphics& graphics)
{
	cool_time = 15.0f;
	life_time = 10.0f;
}

void PhysicalUp::update(Graphics& graphics, float elapsed_time)
{
	life_time -= elapsed_time;
	if (life_time < 0)
	{
		skill_end_flag = true;
	}
}

void PhysicalUp::render(Graphics& graphics)
{
}

void PhysicalUp::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "PhysicalUp:" + str_id + "Param";
	ImGui::Begin("PhysicalUp");
	ImGui::PushID(str_id.c_str());
	/*これより下にパラメーター記述*/
	ImGui::BulletText(name.c_str());
	ImGui::DragFloat("life_time", &life_time);
	ImGui::DragFloat("enhanced_jump_speed", &(*enhanced_jump_speed));
	
	/*これより上にパラメーター記述*/
	ImGui::PopID();
	ImGui::End();

#endif

}
