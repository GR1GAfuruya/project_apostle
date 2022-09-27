#include "skill.h"
#include "imgui_include.h"
#include "user.h"
Skill::Skill(Graphics& graphics)
{
	initialize(graphics);
}

//初期化
void Skill::initialize(Graphics& graphics)
{
	life_time = 5;
}


//更新
void Skill::update(Graphics& graphics, float elapsed_time)
{
	life_time -= elapsed_time;
}

//ｂ
void Skill::render(Graphics& graphics)
{
}

void Skill::debug_gui(const char* str_id)
{
#if USE_IMGUI
	string name = "Skill:" + to_string(*str_id) + "Param";
	imgui_menu_bar("charactor", "skill", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("Player");
		ImGui::PushID(str_id);
		/*これより下にパラメーター記述*/
		if (ImGui::CollapsingHeader("SKill", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat("life_time", &life_time);
		}
		/*これより上にパラメーター記述*/
		ImGui::PopID();
		ImGui::End();
	}
#endif
}
