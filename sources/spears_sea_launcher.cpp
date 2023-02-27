#include "spears_sea_launcher.h"
#include "collision.h"
//==============================================================
// 
// コンストラクタ
// 
//==============================================================
SpearSeaLauncher::SpearSeaLauncher(Graphics& graphics)
{
	init_param.radius = 15;
	init_param.collider_radius = 8;
	init_param.skill_duration = 1.5f;
	//槍の長さ
	init_param.spear_length = 1.0f;
	//長さの伸び率
	init_param.spear_length_rate = 14.0f;
	//ターゲットに向かうスピード
	init_param.speed = 7.5f;
	//追従する時間
	init_param.follow_time = 0.2f;
	//攻撃パラメーター初期化
	atk_param.power = 20;
	atk_param.invinsible_time = 0.1f;
	atk_param.camera_shake.max_x_shake = 4.0f;
	atk_param.camera_shake.max_y_shake = 12.0f;
	atk_param.camera_shake.time = 0.4f;
	atk_param.hit_stop.time = 0.4f;
	atk_param.hit_stop.stopping_strength = 5.0f;
	//クールタイム
	max_cool_time = 15.0f;

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
void SpearSeaLauncher::skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera)
{
	for (auto& s : skills)
	{
		if(!s->is_skill_hit())
		{
			if (Collision::sphere_vs_capsule(s->get_colider().start, s->get_colider().radius,
				object_colider.start, object_colider.end, object_colider.radius))
			{
				//スキルがヒット
				s->skill_hit();
				s->set_is_skill_hit(true);
				//カメラシェイク
				camera->set_camera_shake(atk_param.camera_shake);
				//camera->set_hit_stop(atk_param.hit_stop);
				//ダメージを与える
				damaged_func(atk_param.power, atk_param.invinsible_time, WINCE_TYPE::NONE);
			}
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
			ImGui::DragFloat("max_cool_time", &max_cool_time);
			ImGui::DragFloat("life_span", &init_param.skill_duration);
			ImGui::DragInt("power", &atk_param.power);
			ImGui::DragFloat("invinsible_time", &atk_param.invinsible_time);
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
