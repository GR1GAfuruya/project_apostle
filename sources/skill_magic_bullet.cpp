#include "skill_magic_bullet.h"
#include "Operators.h"

 MagicBullet::MagicBullet(Graphics& graphics, DirectX::XMFLOAT3 init_pos, DirectX::XMFLOAT3 dir, InitializeParam init_param)
{
	 initialize(graphics);
	

	 power = init_param.power;
	 invinsible_time = init_param.invisible_time;
	 acceleration = init_param.acceleration;
	 attack_colider.radius = init_param.collider_radius;
	 cool_time = init_param.cool_time;

	 position = init_pos;
	 velocity = acceleration * Math::Normalize(dir);
	 //エフェクト
	 main_effect = std::make_unique<MeshEffect>(graphics, "./resources/Model/Sphere.fbx");
	// main_effect = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_spear.fbx");
	 main_effect->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill2_output.png");
	 main_effect->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/T_Perlin_Noise_M.tga");
	 main_effect->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga");
	 main_effect->create_pixel_shader(graphics.get_device().Get(), "./shaders/fire_distortion.cso");
	 main_effect->set_scale(0.5f);
	 main_effect->set_life_span(2);
	 main_effect->play(position);
	 main_effect->rotate_base_axis(MeshEffect::AXIS::FORWARD, Math::Normalize(dir));
	 main_effect->constants->data.particle_color = { 1.0f,0.8f,5.5f,1.0f };
}

void MagicBullet::initialize(Graphics& graphics)
{
	life_time = 5;
	collision_type = CollisionType::SPHERE;
	
}

void MagicBullet::update(Graphics& graphics, float elapsed_time)
{
	
	velocity *= 1.05f;
	position += velocity * elapsed_time;
	attack_colider.start = position;

	//エフェクト更新
	main_effect->set_position(position);
	
	main_effect->update(graphics, elapsed_time);
	main_effect->set_is_loop(true);

	//消滅処理
	life_time -= elapsed_time;
	if(is_hit) skill_end_flag = true;
	if (life_time < 0) skill_end_flag = true;
}

void MagicBullet::render(Graphics& graphics)
{
	main_effect->render(graphics);
}

void MagicBullet::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "MagicBullet:" + str_id + "Param";
	ImGui::Begin("MagicBullet");
	ImGui::PushID(str_id.c_str());
	/*これより下にパラメーター記述*/
	ImGui::BulletText(name.c_str());
	ImGui::DragFloat3("position", &position.x);
	ImGui::DragFloat3("velocity", &velocity.x);
	ImGui::DragFloat("life_time", &life_time);
	

	/*これより上にパラメーター記述*/
	ImGui::PopID();
	ImGui::End();

#endif

}
