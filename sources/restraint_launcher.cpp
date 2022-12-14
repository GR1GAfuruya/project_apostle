#include "restraint_launcher.h"
#include "skill_restraint.h"
//==============================================================
// 
// コンストラクタ
// 
//==============================================================
RestraintLauncher::RestraintLauncher(Graphics& graphics)
{
}
//==============================================================
// 
// スキル発動
// 
//==============================================================
bool RestraintLauncher::chant(Graphics& graphics, DirectX::XMFLOAT3* target_pos, DirectX::XMFLOAT3* down_speed)
{
	//詠唱可能な状態なら
	if (chantable)
	{
		unique_ptr<Skill> skill = make_unique<Restrain>();
		max_cool_time = skill->get_cool_time();
		cool_time = max_cool_time;
		//リストに追加
		skills.push_back(std::move(skill));
		chantable = false;
		return true;
	}
	return false;
}
//==============================================================
// 
//デバッグGUI表示
// 
//==============================================================
void RestraintLauncher::debug_gui()
{
#if USE_IMGUI
	imgui_menu_bar("Skill", "Restraint", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("Restraint");

		ImGui::End();
	}
#endif


}
