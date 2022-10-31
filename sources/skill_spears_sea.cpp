#include "skill_spears_sea.h"
#include "noise.h"
#include "user.h"
SpearsSea::SpearsSea(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, InitializeParam initparam)
{
	position = launch_pos;

	instance_mesh = std::make_unique<InstanceMeshEffect>(graphics, "./resources/Effects/Meshes/eff_spear.fbx",MAX_NUM);
	instance_mesh->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill2_output.png");
	instance_mesh->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/T_Perlin_Noise_M.tga");
	instance_mesh->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga");
	instance_mesh->create_pixel_shader(graphics.get_device().Get(), "./shaders/fire_distortion.cso");
	instance_mesh->constants->data.particle_color = { 1.0f,0.8f,5.5f,1.0f };
	power = initparam.power;
	invinsible_time = initparam.invinsible_time;
	cool_time = initparam.cool_time;
	radius = initparam.radius;
	dir = initparam.dir;
	//power = initparam.power;
	//寿命を設定
	life_span = 50.0f;
}

void SpearsSea::initialize(Graphics& graphics)
{
}

void SpearsSea::update(Graphics& graphics, float elapsed_time)
{
	life_time += elapsed_time;
	//槍エフェクトの再生
	if (life_time > 0.5f)
	{
		DirectX::XMFLOAT3 appearance_pos{};
		//すでに再生状態なら更新しない
		if (!instance_mesh->get_active())
		{
			for (int i = 0; i < MAX_NUM; i++)
			{
				instance_mesh->play(appearance_pos);

				int random = std::abs(static_cast<int>(Noise::instance().get_rnd()));
				float circle_radius = random % static_cast<int>(radius);
				appearance_pos.x = Math::circumferential_placement({ position.x,position.z }, circle_radius, i, MAX_NUM).x;
				appearance_pos.y = position.y;
				appearance_pos.z = Math::circumferential_placement({ position.x,position.z }, circle_radius, i, MAX_NUM).y;
				//ばらばらに生やす
				DirectX::XMFLOAT3 spear_dir = Math::Normalize(DirectX::XMFLOAT3(abs(sin(i)), -1.01f, sin(i)));
				instance_mesh->set_position(appearance_pos, i);
				instance_mesh->set_scale(1, i);
				instance_mesh->set_orientation(instance_mesh->rotate_base_axis(InstanceMeshEffect::AXIS::FORWARD, spear_dir, i), i);
				instance_mesh->set_is_loop(true);
			}
		}
	}

	//槍エフェクトの更新

	if (life_time > life_span) skill_end_flag = true;
}

void SpearsSea::render(Graphics& graphics)
{
	for (int i = 0; i < MAX_NUM; i++)
	{
		//main_effect[i]->render(graphics);
	}
	instance_mesh->render(graphics);
}

void SpearsSea::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "SpearsSea:" + str_id + "Param";
	string skill_name = "SpearsSea";
	ImGui::Begin(skill_name.c_str());

	ImGui::PushID(str_id.c_str());
	/*これより下にパラメーター記述*/
	ImGui::BulletText(name.c_str());
	ImGui::DragFloat("life_time", &life_time);
	ImGui::DragFloat2("spear_dir", &dir.x,0.1f);
	if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		instance_mesh->debug_gui(skill_name);

	}

	/*これより上にパラメーター記述*/
	ImGui::PopID();
	ImGui::End();


#endif
}
