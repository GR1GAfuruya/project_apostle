#include "magic_bullet_launcher.h"
#include "user.h"
#include "collision.h"
#include "noise.h"
//==============================================================
// 
// コンストラクタ
// 
//==============================================================
MagicBulletLauncher::MagicBulletLauncher(Graphics& graphics)
{
	atk_param.power = 5;
	atk_param.invinsible_time = 0.2f;
	skill_init_param.acceleration = 50.0f;
	skill_init_param.collider_radius = 2.0f;
	max_cool_time = 0.5f;

	test_slash_hit = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/slash_ray.fbx");
	test_slash_hit->set_material(MaterialManager::instance().mat_fire_distortion.get());
	test_slash_hit->set_init_scale(2.0f);
	test_slash_hit->set_init_life_duration(0.1f);

	test_slash_hit->set_init_color({ 2.5f,2.5f,5.9f,0.5f });

}
//==============================================================
// 
// 更新
// 
//==============================================================
void MagicBulletLauncher::update(Graphics& graphics, float elapsed_time)
{
	//ベースのアップデート
	SkillLauncher::update(graphics, elapsed_time);

	//エフェクト更新
	test_slash_hit->update(graphics, elapsed_time);

}
//==============================================================
// 
// 描画
// 
//==============================================================
void MagicBulletLauncher::render(Graphics& graphics,Camera* camera)
{
	//ベースの描画
	SkillLauncher::render(graphics,camera);

	//エフェクト更新
	test_slash_hit->render(graphics, camera);

}
//==============================================================
// 
// スキル発動
// 
//==============================================================
bool MagicBulletLauncher::chant(Graphics& graphics, DirectX::XMFLOAT3* init_pos, DirectX::XMFLOAT3* dir)
{
	//詠唱可能な状態なら
	if (chantable)
	{
		unique_ptr<Skill> skill = make_unique<MagicBullet>(graphics, init_pos, dir, skill_init_param);
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
//スキルの当たり判定
// 
//==============================================================
void MagicBulletLauncher::skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera)
{
	for (auto& s : skills)
	{
		if (Collision::sphere_vs_capsule(s->get_colider().start, s->get_colider().radius,
			object_colider.start, object_colider.end, object_colider.radius))
		{
			//スキルがヒット
			s->skill_hit();
			s->set_is_skill_hit(true);
			//カメラシェイク
			camera->set_camera_shake(atk_param.camera_shake);
			//ダメージを与える
			damaged_func(atk_param.power, atk_param.invinsible_time, WINCE_TYPE::NONE);
			//ヒットエフェクト
			if (!test_slash_hit->get_active())
			{
				//ヒットエフェクト
				const float middle = (s->get_colider().end.y - s->get_colider().start.y) / 2.0f;
				const DirectX::XMFLOAT3 hit_effect_pos = { s->get_colider().start.x,middle, s->get_colider().start.z };
				test_slash_hit->play(hit_effect_pos);
				test_slash_hit->set_rotate_quaternion(MeshEffect::AXIS::UP, Noise::instance().random_range(0, 90));
				test_slash_hit->set_rotate_quaternion(MeshEffect::AXIS::FORWARD, Noise::instance().random_range(0, 90));
			}
		}
	}
}

//==============================================================
// 
//デバッグGUI表示
// 
//==============================================================
void MagicBulletLauncher::debug_gui()
{
#if USE_IMGUI
	imgui_menu_bar("Skill", "MagicBullet", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("MagicBullet");
		if (ImGui::CollapsingHeader("MagicBulletLauncher", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat("cool_time", &cool_time);
			ImGui::DragFloat("max_cool_time", &max_cool_time);
			ImGui::DragInt("power", &atk_param.power);
			ImGui::DragFloat("invisible_time", &atk_param.invinsible_time);
			ImGui::DragFloat("acceleration", &skill_init_param.acceleration);
			ImGui::DragFloat("collider_radius", &skill_init_param.collider_radius);
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
