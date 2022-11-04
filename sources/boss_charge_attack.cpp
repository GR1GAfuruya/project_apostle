#include "boss_charge_attack.h"
#include "user.h"
#include <algorithm>
ChargeAttack::ChargeAttack(Graphics& graphics)
{
	//auraの初期設定
	for (int i = 0; i < 2; i++)
	{
		aura[i] = make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_spiral.fbx");
		aura[i]->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill2_output.png");
		aura[i]->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/Mask/dissolve_animation.png");
		aura[i]->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga");
		aura[i]->create_pixel_shader(graphics.get_device().Get(), "./shaders/fire_distortion.cso");
		aura[i]->constants->data.particle_color = FIRE_COLOR;

	}
	//coreの初期設定
	core = make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_sphere.fbx");
	core->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill2_output.png");
	core->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/Mask/dissolve_animation.png");
	core->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga");
	core->create_pixel_shader(graphics.get_device().Get(), "./shaders/fire_distortion.cso");
	core->constants->data.particle_color = FIRE_COLOR;
	
	//waveの初期設定
	wave = std::make_unique<MeshEffect>(graphics,"./resources/Effects/Meshes/eff_aura.fbx");
	wave->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill2_output.png");
	wave->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/Mask/dissolve_animation.png");
	wave->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga");
	wave->create_pixel_shader(graphics.get_device().Get(), "./shaders/fire_distortion.cso");
	//tornadoの初期設定
	tornado = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_tornado4.fbx");
	tornado->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill2_output.png");
	tornado->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/Mask/dissolve_animation.png");
	tornado->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga");
	tornado->create_pixel_shader(graphics.get_device().Get(), "./shaders/fire_distortion.cso");
	tornado->constants->data.particle_color = FIRE_COLOR;
	//定数バッファ初期設定
	constants = std::make_unique<Constants<ChargeAttackConstants>>(graphics.get_device().Get());
	//particleの初期設定
	particle = std::make_unique<GPU_Particles>(graphics.get_device().Get(), 150000);
	particle.get()->initialize(graphics);
	create_cs_from_cso(graphics.get_device().Get(), "shaders/boss_charge_attack_emit.cso", emit_cs.ReleaseAndGetAddressOf());
	create_cs_from_cso(graphics.get_device().Get(), "shaders/boss_charge_attack_update.cso", update_cs.ReleaseAndGetAddressOf());
}

void ChargeAttack::play(DirectX::XMFLOAT3 pos)
{
	//アクティブ状態に
	active = true;
	is_charge_max = false;
	attack_time = 0;
	charge_attack_update = &ChargeAttack::charging_update;
	//タイマーリセット
	life_time = 0.0f;
	//コア位置設定
	position = pos;
	DirectX::XMFLOAT3 core_pos = { position.x,position.y + 70,position.z };
	core->play(core_pos);
	core->set_scale( 0);
	core->set_is_loop(true);
	core->constants->data.particle_color = FIRE_COLOR;
	//core->constants->data.scroll_direction = { 1.0f,-1.0f };
	//wave初期設定
	wave->play(core_pos);
	wave->set_scale(0.0f);
	//wave->constants->data.scroll_direction = { 0.5f,0.8f };
	wave->constants->data.threshold = 0;
	wave->constants->data.particle_color = { FIRE_COLOR.x,FIRE_COLOR.y,FIRE_COLOR.z, 1.0f};
	DirectX::XMFLOAT4 wave_ori = wave->get_orientation();
	//下を向かせる
	//wave->set_orientation(Math::rot_quaternion_dir(wave_ori, Math::get_posture_forward(wave_ori), { 0,-1,0 }));
	wave->set_rotate_quaternion(MeshEffect::AXIS::RIGHT, 90);

	//トルネード初期化
	tornado->play(pos);
	//tornado->set_rotate_quaternion(MeshEffect::AXIS::RIGHT, -90);
	tornado->set_scale(0);
	tornado->constants->data.scroll_direction = { 0.0f,-0.2f };
	tornado->constants->data.threshold = 0;
	tornado->constants->data.particle_color = { FIRE_COLOR.x,FIRE_COLOR.y,FIRE_COLOR.z, 1.0f };
	//コアへ延びるエフェクトのプレイ時の処理　
	//ボス周りに3角形の位置
	for (int i = 0; i < 2; i++)
	{
		aura[i]->play(pos);
		aura[i]->set_is_loop(true);

		DirectX::XMFLOAT4 aura_q = aura[i]->get_orientation();
		DirectX::XMFLOAT3 aura_forward = Math::get_posture_forward(aura_q);
		aura[i]->set_rotate_quaternion(MeshEffect::AXIS::RIGHT, -90);
		//2本のうちの1つの角度をずらして螺旋っぽく

		aura[i]->constants->data.particle_color = FIRE_COLOR;
		//aura[i]->constants->data.scroll_direction.y = 1.5f;
		aura[i]->constants->data.threshold = 0.0f;
		aura[i]->rot_speed.z = 520;
		aura[i]->set_scale({ 2.0f, 2.0f, 1.5f });
	}
	aura[1]->set_rotate_quaternion(MeshEffect::AXIS::FORWARD, 180);

	
	DirectX::XMFLOAT3 emit_pos{};
	emit_pos.y = core_pos.y;
	const float radius = 70.0f;
	for (float theta = 0; theta < 360.0f; theta += 120.0f)
	{
		emit_pos.x = core_pos.x + (radius * sinf(DirectX::XMConvertToRadians(theta)));
		emit_pos.z = core_pos.z + (radius * cosf(DirectX::XMConvertToRadians(theta)));
		particle.get()->set_emitter_pos(emit_pos);
		particle.get()->set_emitter_rate(16);
		particle.get()->set_emitter_life_time(4);
		particle.get()->set_particle_life_time(life_time);
		particle.get()->set_particle_size(DirectX::XMFLOAT2(0.2f,0.2f));
		particle.get()->particle_constants->data.particle_color = FIRE_COLOR;
		particle.get()->launch_emitter(emit_cs);
	}
	
	constants->data.core_pos = core_pos;
}

void ChargeAttack::stop()
{
	active = false;
	for (int i = 0; i < 2; i++)
	{
		aura[i]->stop();
	}
	core->stop();

	wave->stop();
}

void ChargeAttack::update(Graphics& graphics, float elapsed_time)
{
	if (active)
	{
		life_time += elapsed_time;
		//更新
		(this->*charge_attack_update)(graphics, elapsed_time);	
	}
	particle.get()->update(graphics.get_dc().Get(), elapsed_time, update_cs.Get());
}

void ChargeAttack::render(Graphics& graphics)
{

	if (active)
	{
		for (int i = 0; i < 2; i++)
		{
			aura[i]->render(graphics);
		}
		core->render(graphics);


		tornado->render(graphics);
		wave->render(graphics);
		particle->render(graphics.get_dc().Get(), graphics.get_device().Get());
	}
}

void ChargeAttack::debug_gui(const char* str_id)
{
#if USE_IMGUI
	imgui_menu_bar("Effects", "boss_charge", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("boss_charge");
		ImGui::DragFloat3("position", &position.x, 0.1f);
		ImGui::DragFloat("core_gravitation", &constants->data.core_gravitation, 0.1f, 0);
		ImGui::DragFloat("core_radius", &constants->data.core_radius, 1, 0);
		ImGui::End();
	}
#endif
	for (int i = 0; i < 2; i++)
	{
		aura[i]->debug_gui(to_string(i).c_str());
	}
	core->debug_gui("core");

	wave->debug_gui("wave");
	tornado->debug_gui("tornad");
	particle->debug_gui("boss_charge");
}

//チャージ中のアップデート
void ChargeAttack::charging_update(Graphics& graphics, float elapsed_time)
{
	//更新処理
	float core_s = lerp(core->get_scale().x, 0.3f, 0.2f * elapsed_time);
	core->set_scale(core_s);
	core->constants->data.scroll_speed += elapsed_time;
	core->update(graphics, elapsed_time);
	for (int i = 0; i < 2; i++)
	{
		aura[i]->update(graphics, elapsed_time);
		aura[i]->constants->data.scroll_speed += elapsed_time;
		aura[i]->constants->data.threshold = 0.5f;

	}

	//コアの重力設定
	constants->data.core_gravitation = 0.5f;

	constants->bind(graphics.get_dc().Get(), 10, CB_FLAG::CS);
	if (is_charge_max)
	{
		core->set_scale(0.7f);
		charge_attack_update = &ChargeAttack::activities_update;
		tornado->set_scale({ 0.0f,15.0f,0.0f });
	}
}

//チャージが完了し、発動したときのアップデート
void ChargeAttack::activities_update(Graphics& graphics, float elapsed_time)
{
	
	attack_time += elapsed_time;
	auto fade_out = [=](float alpha) {return (std::max)(alpha - 0.5f * elapsed_time, 0.0f); };

	const float core_s = lerp(core->get_scale().x, 0.0f, 3.5f * elapsed_time);
	core->set_scale(core_s);
	core->constants->data.scroll_speed += elapsed_time;
	core->update(graphics, elapsed_time);

	for (int i = 0; i < 2; i++)
	{
		aura[i]->update(graphics, elapsed_time);
		aura[i]->constants->data.scroll_speed += elapsed_time;
		aura[i]->constants->data.particle_color.w = fade_out(aura[i]->constants->data.particle_color.w);
	}
	//コアの重力設定
	//反転させてパーティクルを飛び散らせる
	constants->data.core_gravitation = -0.4f;
	constants->bind(graphics.get_dc().Get(), 10, CB_FLAG::CS);
	DirectX::XMFLOAT3 wave_scale = wave->get_scale();
	//波動エフェクト
	wave_scale.x = lerp(wave_scale.x, 100.0f, 5.0f * elapsed_time);
	wave_scale.y = lerp(wave_scale.y, 100.0f, 5.0f * elapsed_time);
	wave->set_scale(wave_scale);
	wave->constants->data.scroll_speed += elapsed_time;
	wave->set_rotate_quaternion(MeshEffect::AXIS::FORWARD, 360 * elapsed_time);

	//竜巻エフェクト
	float scale_xy = lerp(tornado->get_scale().y, 15.0f, 5.0f * elapsed_time);
	tornado->set_scale({ scale_xy, tornado->get_scale().y, scale_xy });
	//tornado->constants->data.scroll_speed += elapsed_time;
	tornado->set_rotate_quaternion(MeshEffect::AXIS::UP, 260 * elapsed_time);

	if (attack_time >= ATTACK_TIME) charge_attack_update = &ChargeAttack::vanishing_update;
}

//消滅時のアップデート
void ChargeAttack::vanishing_update(Graphics& graphics, float elapsed_time)
{
	//徐々に消えていく関数
	const float fade_out_speed = 7.0f;
	auto fade_out = [=](float alpha) {return (std::max)(alpha - fade_out_speed * elapsed_time, 0.0f); };
	
	wave->constants->data.particle_color.w = fade_out(wave->constants->data.particle_color.w);
	//wave->constants->data.scroll_speed += elapsed_time;
	const float threshold_speed = 2.0f;
	wave->constants->data.threshold = (std::min)(wave->constants->data.threshold + threshold_speed * elapsed_time, 1.0f);
	wave->set_rotate_quaternion(MeshEffect::AXIS::FORWARD, 520 * elapsed_time);


	float expand = 15 * elapsed_time;
	tornado->set_scale({ tornado->get_scale().x + expand,tornado->get_scale().y + expand,tornado->get_scale().z + expand });
	tornado->constants->data.particle_color.w = fade_out(tornado->constants->data.particle_color.w);
	tornado->set_rotate_quaternion(MeshEffect::AXIS::UP, 360 * elapsed_time);
	tornado->constants->data.threshold = (std::min)(tornado->constants->data.threshold + threshold_speed * elapsed_time, 1.0f);
	
	if (tornado->constants->data.threshold > 0.9f) stop();
}
