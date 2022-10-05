#include "skill_spears_sea.h"

SpearsSea::SpearsSea(Graphics& graphics, DirectX::XMFLOAT3 launch_pos)
{
	pos = launch_pos;
}

void SpearsSea::initialize(Graphics& graphics)
{
	life_time = 5.0f;
	cool_time = 10.0f;
}

void SpearsSea::update(Graphics& graphics, float elapsed_time)
{
	life_time -= elapsed_time;
	if (life_time < 0) skill_end_flag = true;
}

void SpearsSea::render(Graphics& graphics)
{
}

void SpearsSea::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "SpearsSea:" + str_id + "Param";
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
