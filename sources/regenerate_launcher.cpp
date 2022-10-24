#include "regenerate_launcher.h"
#include "skill_regenerate.h"
RegenerateLauncher::RegenerateLauncher(Graphics& graphics)
{
}

bool RegenerateLauncher::chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, int* health)
{
	//‰r¥‰Â”\‚Èó‘Ô‚È‚ç
	if (chantable)
	{
	unique_ptr<Skill> skill = make_unique<Regenerate>();
		//ƒŠƒXƒg‚É’Ç‰Á
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
