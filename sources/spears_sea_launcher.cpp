#include "spears_sea_launcher.h"

#include "collision.h"
SpearSeaLauncher::SpearSeaLauncher(Graphics& graphics)
{
	init_param.power = 5;
	init_param.invinsible_time = 3.0f;
	init_param.radius = 15;
	init_param.collider_radius = 8;
}

bool SpearSeaLauncher::chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos)
{
	//‰r¥‰Â”\‚Èó‘Ô‚È‚ç
	if (chantable)
	{
	std:unique_ptr<Skill> skill = make_unique<SpearsSea>(graphics, launch_pos, init_param);
		//ƒŠƒXƒg‚É’Ç‰Á
		cool_time = skill->get_cool_time();
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
		if (ImGui::CollapsingHeader("SpearSeaLauncher", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat("cool_time", &cool_time);
			ImGui::DragFloat("init_cool_time", &init_param.cool_time);
			ImGui::DragFloat("power", &init_param.power);
			ImGui::DragFloat("invinsible_time", &init_param.invinsible_time);
			ImGui::DragFloat("radius", &init_param.radius);
			ImGui::DragFloat("collider_radius", &init_param.collider_radius);
			ImGui::Separator();
			int count = 0;
			for (auto& s : skills)
			{
				s->debug_gui(to_string(count).c_str());
				count++;
			}
		}
		ImGui::End();
	}
#endif

}
