#include "regenerate_launcher.h"
#include "skill_regenerate.h"
RegenerateLauncher::RegenerateLauncher()
{
	max_cool_time = 15.0f;
}

bool RegenerateLauncher::chant(DirectX::XMFLOAT3* launch_pos, int* health, const int max_health)
{
	//詠唱可能な状態なら
	if (chantable)
	{
		//スキルのパラメーター設定
		unique_ptr<Skill> skill = make_unique<Regenerate>(launch_pos, health, max_health);
		cool_time = max_cool_time;
		//リストに追加
		skills.push_back(std::move(skill));

		chantable = false;
		return true;
	}
	return false;
}

void RegenerateLauncher::debug_gui()
{
#if USE_IMGUI
	imgui_menu_bar("Skill", "Regenerate", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("Regenerate");

		ImGui::End();
	}
#endif

}
