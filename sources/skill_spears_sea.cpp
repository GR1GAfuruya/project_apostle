#include "skill_spears_sea.h"
#include "noise.h"
#include "user.h"
SpearsSea::SpearsSea(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, InitializeParam initparam)
{
	position = launch_pos;

	for (int i = 0; i < MAX_NUM; i++)
	{
		main_effect[i] = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_spear.fbx");
		main_effect[i]->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill2_output.png");
		main_effect[i]->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/T_Perlin_Noise_M.tga");
		main_effect[i]->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga");
		main_effect[i]->create_pixel_shader(graphics.get_device().Get(), "./shaders/fire_distortion.cso");
		main_effect[i]->set_scale(0.5f);
		main_effect[i]->set_life_span(2);
		
		main_effect[i]->constants->data.particle_color = { 1.0f,0.8f,5.5f,1.0f };
	};
	power = initparam.power;
	invinsible_time = initparam.invinsible_time;
	cool_time = initparam.cool_time;
	radius = initparam.radius;
	//power = initparam.power;
	//寿命を設定
	life_span = 5.0f;
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
		DirectX::XMFLOAT3 appearance_pos;
		for (int i = 0; i < MAX_NUM ; i++)
		{
			if (main_effect[i]->get_active() == true) continue; //すでに再生状態なら更新しない

			int random = Noise::instance().get_rnd();
			float circle_radius = random % static_cast<int>(radius);
			appearance_pos.x = Math::circumferential_placement({ position.x,position.z }, circle_radius, i, MAX_NUM).x;
			appearance_pos.y = position.y;
			appearance_pos.z = Math::circumferential_placement({ position.x,position.z }, circle_radius, i, MAX_NUM).y;
			main_effect[i]->play(appearance_pos);
			float random_dir = random % 10 / 10.0f - 0.5f; //-0.5から0.5の値
			DirectX::XMFLOAT3 spear_dir = Math::Normalize(DirectX::XMFLOAT3(random_dir, 1.01f, random_dir));
			main_effect[i]->rotate_base_axis(MeshEffect::AXIS::FORWARD, spear_dir);
			main_effect[i]->set_is_loop(true);
		}
	}
	//槍エフェクトの更新
	for (int i = 0; i < MAX_NUM; i++)
	{
		main_effect[i]->update(graphics, elapsed_time);
	}

	if (life_time > life_span) skill_end_flag = true;
}

void SpearsSea::render(Graphics& graphics)
{
	for (int i = 0; i < MAX_NUM; i++)
	{
		main_effect[i]->render(graphics);
	}
}

void SpearsSea::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "SpearsSea:" + str_id + "Param";
	ImGui::Begin("SpearsSea");

	ImGui::PushID(str_id.c_str());
	/*これより下にパラメーター記述*/
	ImGui::BulletText(name.c_str());
	ImGui::DragFloat("life_time", &life_time);
	

	/*これより上にパラメーター記述*/
	ImGui::PopID();
	ImGui::End();

#endif
}
