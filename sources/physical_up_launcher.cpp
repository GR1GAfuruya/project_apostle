#include "physical_up_launcher.h"
#include "skill_physical_up.h"
PhycicalUpLauncher::PhycicalUpLauncher(Graphics& graphics)
{
}

bool PhycicalUpLauncher::chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, float* add_run_speed, float* add_jump_speed)
{
	//ârè•â¬î\Ç»èÛë‘Ç»ÇÁ
	if (chantable)
	{
		unique_ptr<Skill> skill = make_unique<PhysicalUp>(graphics, launch_pos, add_run_speed, add_jump_speed);
		//ÉäÉXÉgÇ…í«â¡
		skills.push_back(std::move(skill));
		chantable = false;
		return true;
	}
	return false;
}

void PhycicalUpLauncher::debug_gui()
{
#if USE_IMGUI
	ImGui::Begin("Skill");
	if (ImGui::CollapsingHeader("PhycicalUpLauncher", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat("cool_time", &cool_time);
		int count = 0;
		for (auto& s : skills)
		{
			s->debug_gui(to_string(count).c_str());
			count++;
			ImGui::Separator();
		}


	}
	ImGui::End();
#endif
}
