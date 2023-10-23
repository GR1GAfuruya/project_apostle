#include "physical_up_launcher.h"
#include "skill_physical_up.h"
//==============================================================
// 
// コンストラクタ
// 
//==============================================================
PhysicalUpLauncher::PhysicalUpLauncher()
{
	max_cool_time = 5;
}
//==============================================================
// 
// スキル発動
// 
//==============================================================
bool PhysicalUpLauncher::chant(DirectX::XMFLOAT3* launch_pos, float* add_run_speed, float* add_jump_speed)
{
	//詠唱可能な状態なら
	if (chantable)
	{
		unique_ptr<Skill> skill = make_unique<PhysicalUp>(launch_pos, add_run_speed, add_jump_speed);
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
void PhysicalUpLauncher::debug_gui()
{
#if USE_IMGUI
	imgui_menu_bar("Skill", "PhysicalUp", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("PhysicalUp");
		if (ImGui::CollapsingHeader("PhycicalUpLauncher", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PushID("PhycicalUpLauncher");
			ImGui::DragFloat("cool_time", &cool_time);
			int count = 0;
			ImGui::PopID();
			for (auto& s : skills)
			{
				s->debug_gui(to_string(count).c_str());
				count++;
				ImGui::Separator();
			}


		}
		ImGui::End();
	}
#endif
}
