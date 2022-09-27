#include "skill.h"
#include "imgui_include.h"
#include "user.h"
Skill::Skill(Graphics& graphics)
{
	initialize(graphics);
}

void Skill::initialize(Graphics& graphics)
{
	life_time = 5;
}

void Skill::update(Graphics& graphics, float elapsed_time)
{
	life_time -= elapsed_time;
}

void Skill::render(Graphics& graphics)
{
}

void Skill::debug_gui(const char* str_id)
{
#if USE_IMGUI
	string name = "skill:" + to_string(*str_id);
	imgui_menu_bar("skills", name, display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("skill");
		ImGui::PushID(str_id);
		/*これより下にパラメーター記述*/
		ImGui::DragFloat("life_time", &life_time);
		/*これより上にパラメーター記述*/
		ImGui::PopID();
		ImGui::End();
	}
#endif
}
