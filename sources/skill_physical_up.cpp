#include "skill_physical_up.h"

void PhysucalUp::initialize(Graphics& graphics)
{
	life_time = 5.0f;
}

void PhysucalUp::update(Graphics& graphics, float elapsed_time)
{
	life_time -= elapsed_time;
	if (life_time < 0) skill_end_flag = true;
}

void PhysucalUp::render(Graphics& graphics)
{
}

void PhysucalUp::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "PhysucalUp:" + str_id + "Param";
	ImGui::Begin("Skill");
	ImGui::PushID(str_id.c_str());
	/*これより下にパラメーター記述*/
	ImGui::BulletText(name.c_str());
	ImGui::DragFloat("life_time", &life_time);
	
	/*これより上にパラメーター記述*/
	ImGui::PopID();
	ImGui::End();

#endif

}
