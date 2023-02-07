#include "slash_wave_launcher.h"

SlashWaveLauncher::SlashWaveLauncher(Graphics& graphics)
{
	atk_param.power = 5;
	atk_param.invinsible_time = 0.2f;
	init_param.speed = 500.0f;
	init_param.collider_radius = 5.0f;
	max_cool_time = 0.5f;
}

bool SlashWaveLauncher::chant(Graphics& graphics, DirectX::XMFLOAT3* launch_pos, DirectX::XMFLOAT3* dir)
{
	//詠唱可能な状態なら
	if (chantable)
	{
		std:unique_ptr<Skill> skill = make_unique<SlashWave>(graphics, launch_pos, dir, init_param);
		//リストに追加
		cool_time = max_cool_time;
		skills.push_back(std::move(skill));
		chantable = false;
		return true;
	}
	return false;
}

void SlashWaveLauncher::skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera)
{
	for (auto& s : skills)
	{
		if (Collision::sphere_vs_capsule(s->get_colider().start ,s->get_colider().radius,
			object_colider.start, object_colider.end, object_colider.radius))
		{
			//スキルがヒット
			s->skill_hit();
			s->set_is_skill_hit(true);
			//カメラシェイク
			camera->set_camera_shake(atk_param.camera_shake);
			//ダメージを与える
			damaged_func(atk_param.power, atk_param.invinsible_time, WINCE_TYPE::NONE);
		}
	}
}

void SlashWaveLauncher::debug_gui()
{
#if USE_IMGUI
	imgui_menu_bar("Skill", "SlashWave", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("SlashWave");
		if (ImGui::CollapsingHeader("SlashWaveLauncher", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat("cool_time", &cool_time);
			ImGui::DragFloat("max_cool_time", &max_cool_time);
			ImGui::DragInt("power", &atk_param.power);
			ImGui::DragFloat("invisible_time", &atk_param.invinsible_time);
			ImGui::DragFloat("acceleration", &init_param.speed);
			ImGui::DragFloat("collider_radius", &init_param.collider_radius);
			int count = 0;
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
