#include "spears_sea_launcher.h"
#include "collision.h"
//==============================================================
// 
// コンストラクタ
// 
//==============================================================
SpearSeaLauncher::SpearSeaLauncher(Graphics& graphics)
{
	init_param.power = 10;
	init_param.invinsible_time = 2.5f;
	init_param.radius = 15;
	init_param.collider_radius = 8;
	init_param.cool_time = 3.0f;
	init_param.life_span = 2.0f;
	//槍の長さ
	init_param.spear_length = 1.0f;
	//長さの伸び率
	init_param.spear_length_rate = 14.0f;
	//ターゲットに向かうスピード
	init_param.speed = 7.5f;
	//追従する時間
	init_param.follow_time = 0.2f;
}
//==============================================================
// 
// 発動
// 
//==============================================================
bool SpearSeaLauncher::chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos)
{
	//詠唱可能な状態なら
	if (chantable)
	{
	std:unique_ptr<Skill> skill = make_unique<SpearsSea>(graphics, launch_pos, target_pos, init_param);
		//リストに追加
		max_cool_time = skill->get_cool_time();
		cool_time = max_cool_time;
		skills.push_back(std::move(skill));
		chantable = false;
		return true;
	}
	return false;
}
//==============================================================
// 
// 当たり判定
// 
//==============================================================
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
//==============================================================
// 
// デバッグGUI
// 
//==============================================================
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
			ImGui::DragFloat("life_span", &init_param.life_span);
			ImGui::DragFloat("power", &init_param.power);
			ImGui::DragFloat("invinsible_time", &init_param.invinsible_time);
			ImGui::DragFloat("radius", &init_param.radius);
			ImGui::DragFloat("collider_radius", &init_param.collider_radius);
			ImGui::DragFloat("spear_length", &init_param.spear_length, 0.1f);
			ImGui::DragFloat("spear_length_rate", &init_param.spear_length_rate, 0.1f);
			ImGui::DragFloat("speed", &init_param.speed, 0.1f);
			ImGui::DragFloat("follow_time", &init_param.follow_time, 0.1f);

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
