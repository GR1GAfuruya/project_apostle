#include "skill_physical_up.h"

void PhysicalUp::initialize(Graphics& graphics)
{
	life_time = 5.0f;
}

void PhysicalUp::update(Graphics& graphics, float elapsed_time)
{
	life_time -= elapsed_time;
	if (life_time < 0) skill_end_flag = true;
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
	
	/*これより上にパラメーター記述*/
	ImGui::PopID();
	ImGui::End();

#endif

}
