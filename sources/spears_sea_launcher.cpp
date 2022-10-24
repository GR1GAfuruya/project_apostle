#include "spears_sea_launcher.h"
#include "skill_spears_sea.h"
#include "collision.h"
SpearSeaLauncher::SpearSeaLauncher(Graphics& graphics)
{
}

bool SpearSeaLauncher::chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos)
{
	//ârè•â¬î\Ç»èÛë‘Ç»ÇÁ
	if (chantable)
	{
	std:unique_ptr<Skill> skill = make_unique<SpearsSea>(graphics, launch_pos);
		//ÉäÉXÉgÇ…í«â¡
		skills.push_back(std::move(skill));
		chantable = false;
		return true;
	}
	return false;
}

void SpearSeaLauncher::skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func)
{
	for (auto& s : skills)
	{
		if (Collision::sphere_vs_capsule(s->get_colider().start, s->get_colider().radius,
			object_colider.start, object_colider.end, object_colider.radius))
		{
			s->skill_hit();
			damaged_func(s->get_power(), s->get_invinsible_time(),WINCE_TYPE::SMALL);
		}
	}
}

void SpearSeaLauncher::debug_gui()
{
#if USE_IMGUI
	imgui_menu_bar("Skill", "SpearSea", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("SpearSea");

		ImGui::End();
	}
#endif

}
