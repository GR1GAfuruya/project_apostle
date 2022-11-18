#include "skill_regenerate.h"
//==============================================================
// 
//初期化
// 
//==============================================================
void Regenerate::initialize(Graphics& graphics)
{
	life_time = 5.0f;
}
//==============================================================
// 
//更新
// 
//==============================================================
void Regenerate::update(Graphics& graphics, float elapsed_time)
{
	life_time -= elapsed_time;
	if (life_time < 0)
	{
		skill_end_flag = true;
	}
}
//==============================================================
// 
//描画
// 
//==============================================================
void Regenerate::render(Graphics& graphics)
{
}
//==============================================================
// 
//デバッグGUI
// 
//==============================================================
void Regenerate::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "Regenerate:" + str_id + "Param";
	ImGui::Begin("Regenerate");
	ImGui::PushID(str_id.c_str());
	/*これより下にパラメーター記述*/
	ImGui::BulletText(name.c_str());

	ImGui::DragFloat("life_time", &life_time);
	ImGui::Checkbox("skill_end_flag", &skill_end_flag);
	
	/*これより上にパラメーター記述*/
	ImGui::PopID();
	ImGui::End();

#endif

}
