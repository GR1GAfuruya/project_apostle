#include "boss_charge_attack.h"
#include "user.h"
#include "noise.h"
#include "light_manager.h"
//==============================================================
// 
//コンストラクタ
// 
//==============================================================
ChargeAttack::ChargeAttack(Graphics& graphics)
{
	//coreの初期設定
	core = make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_sphere.fbx");
	core->set_material(MaterialManager::instance().mat_fire_distortion.get());
	core->constants->data.particle_color = FIRE_COLOR;
	
	//waveの初期設定
	wave = std::make_unique<MeshEffect>(graphics,"./resources/Effects/Meshes/eff_aura.fbx");
	wave->set_material(MaterialManager::instance().mat_fire_distortion.get());
	//tornadoの初期設定
	tornado = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_tornado4.fbx");
	tornado->set_material(MaterialManager::instance().mat_fire_distortion.get());
	tornado->constants->data.particle_color = FIRE_COLOR;
	//定数バッファ初期設定
	constants = std::make_unique<Constants<ChargeAttackConstants>>(graphics.get_device().Get());
	//particleの初期設定
	particle = std::make_unique<GPU_Particles>(graphics.get_device().Get(), 100000);

	meteores = std::make_unique<Meteore>(graphics, 10);
	particle.get()->initialize(graphics);
	create_cs_from_cso(graphics.get_device().Get(), "shaders/boss_charge_attack_emit.cso", emit_cs.ReleaseAndGetAddressOf());
	create_cs_from_cso(graphics.get_device().Get(), "shaders/boss_charge_attack_update.cso", update_cs.ReleaseAndGetAddressOf());
	meteo_span = ATTACK_TIME / (meteores->get_max_num() + 1);
	meteo_launch_radius = 5;

	boss_light = make_shared<PointLight>(graphics, position, 20.0f, DirectX::XMFLOAT3(FIRE_COLOR.x, FIRE_COLOR.y, FIRE_COLOR.z));

}
//==============================================================
// 
//コンストラクタ
// 
//==============================================================
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
	//wave初期設定
	wave->play(core_pos);
	wave->set_scale(0.0f);
	wave->constants->data.threshold = 0;
	wave->constants->data.particle_color = { FIRE_COLOR.x,FIRE_COLOR.y,FIRE_COLOR.z, 1.0f};
	DirectX::XMFLOAT4 wave_ori = wave->get_orientation();
	//下を向かせる
	wave->set_rotate_quaternion(MeshEffect::AXIS::RIGHT, 90);

	//トルネード初期化
	tornado->play(pos);
	tornado->set_scale(0);
	tornado->constants->data.scroll_direction = { 0.0f,-0.2f };
	tornado->constants->data.threshold = 0;
	tornado->constants->data.particle_color = { FIRE_COLOR.x,FIRE_COLOR.y,FIRE_COLOR.z, 1.0f };
	
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

	//隕石
	meteo_time = 0;
	meteores->initialize();
	meteo_launch_count = 0;
	for (int i = 0; i < meteores->get_max_num(); i++)
	{
		const int range = 20;
		const int ofset = meteo_launch_radius;
		const float random = fabs(Noise::instance().random_range(ofset, range));
		meteores->create_on_circle(position, random, i);
	}
	constants->data.core_pos = core_pos;
	boss_light->set_position({ position.x,position.y + 10.0f,position.z });
	LightManager::instance().register_light("charge_light", boss_light);
}
//==============================================================
// 
//コンストラクタ
// 
//==============================================================
void ChargeAttack::stop()
{
	active = false;
	core->stop();

	wave->stop();

	LightManager::instance().delete_light(boss_light->name);
}
//==============================================================
// 
//更新
// 
//==============================================================
void ChargeAttack::update(Graphics& graphics, float elapsed_time)
{
	if (active)
	{
		life_time += elapsed_time;
		//更新
		(this->*charge_attack_update)(graphics, elapsed_time);	
	}
	meteores->update(graphics, elapsed_time);

	particle.get()->update(graphics.get_dc().Get(), elapsed_time, update_cs.Get());
}
//==============================================================
// 
//描画
// 
//==============================================================
void ChargeAttack::render(Graphics& graphics)
{

	if (active)
	{
		core->render(graphics);


		tornado->render(graphics);
		wave->render(graphics);
	}

	//隕石描画
	meteores->render(graphics);

	particle->render(graphics.get_dc().Get(), graphics.get_device().Get());

}
//==============================================================
// 
//コンストラクタ
// 
//==============================================================
void ChargeAttack::debug_gui(const char* str_id)
{
#if USE_IMGUI
	imgui_menu_bar("Effects", "boss_charge", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("boss_charge");
		ImGui::DragFloat3("position", &position.x, 0.1f);
		ImGui::DragFloat("core_gravitation", &constants->data.core_gravitation, 0.1f, 0);
		ImGui::DragFloat("meteo_span", &meteo_span, 0.1f, 0);
		ImGui::DragFloat("core_radius", &constants->data.core_radius, 1, 0);
		ImGui::DragFloat("launch_radius", &meteo_launch_radius, 1, 0);
		ImGui::End();
	}
#endif
	core->debug_gui("core");

	wave->debug_gui("wave");
	tornado->debug_gui("tornad");
	particle->debug_gui("boss_charge");
}

//==============================================================
// 
//プレイヤーとの当たり判定
// 
//==============================================================
void ChargeAttack::calc_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func)
{
	//メテオの当たり判定
	meteores->calc_meteore_vs_player(capsule_start, capsule_end, colider_radius, damaged_func);
}
//==============================================================
// 
//チャージ中のアップデート
// 
//==============================================================
void ChargeAttack::charging_update(Graphics& graphics, float elapsed_time)
{
	//更新処理
	float core_scale = lerp(core->get_scale().x, 0.3f, 0.2f * elapsed_time);
	core->set_scale(core_scale);
	core->constants->data.scroll_speed += elapsed_time;
	core->update(graphics, elapsed_time);

	//コアの重力設定
	constants->data.core_gravitation = 0.5f;

	constants->bind(graphics.get_dc().Get(), 10, CB_FLAG::CS);

	//隕石を地面から浮き上がらせる
	int range = 9;
	int ofset = 1;
	for (int i = 0; i < meteores->get_max_num(); i++)
	{
		float random = fabs(Noise::instance().random_range(ofset, range) / 10.0f);
		range = 3;
		ofset = 4;
		float random_size = fabs(Noise::instance().random_range(ofset, range));
		meteores->rising(elapsed_time, core->get_position(), random_size, random, i);
	}

	//チャージ完了時の処理
	if (is_charge_max)
	{
		core->set_scale(0.7f);
		charge_attack_update = &ChargeAttack::activities_update;
		tornado->set_scale({ 0.0f,15.0f,0.0f });
	}
}
//==============================================================
// 
//チャージが完了し、発動したときのアップデート
// 
//==============================================================
void ChargeAttack::activities_update(Graphics& graphics, float elapsed_time)
{
	
	auto fade_out = [=](float alpha) {return (std::max)(alpha - 0.5f * elapsed_time, 0.0f); };

	const float core_s = lerp(core->get_scale().x, 0.0f, 3.5f * elapsed_time);
	core->set_scale(core_s);
	core->constants->data.scroll_speed += elapsed_time;
	core->update(graphics, elapsed_time);

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
	tornado->set_rotate_quaternion(MeshEffect::AXIS::UP, 260 * elapsed_time);

	attack_time += elapsed_time;
	//メテオ発生
	if (attack_time <= ATTACK_TIME || meteo_launch_count < meteores->get_max_num())
	{
		meteo_time += elapsed_time;
		if (meteo_time >= meteo_span )
		{
			//power
			const int range = 50;
			const int ofset = 50;
			const int random = Noise::instance().random_range(ofset, range);
			float power = random;
			//方向
			DirectX::XMFLOAT3 direction = Math::circumferential_placement(core->get_position(),
				power,meteo_launch_count, meteores->get_max_num());
			//正規化
			direction = Math::Normalize(direction);
			//射出
			meteores->launch(direction, power, meteo_launch_count);
			//間隔をあけるためのタイマーをリセット
			meteo_time = 0;
			//射出したカウントを増やす
			meteo_launch_count++;
		};

		
	}
	if (attack_time >= ATTACK_TIME) charge_attack_update = &ChargeAttack::vanishing_update;
}
//==============================================================
// 
//消滅時のアップデート
// 
//==============================================================
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
