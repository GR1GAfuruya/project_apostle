#include "boss_charge_attack.h"
#include "user.h"
#include "noise.h"
#include "light_manager.h"
//==============================================================
// 
//コンストラクタ
// 
//==============================================================
ChargeAttack::ChargeAttack()
{
	Graphics& graphics = Graphics::instance();
	//coreの初期設定
	{
		core = make_unique<MeshEffect>("./resources/Effects/Meshes/eff_sphere.fbx");
		core->set_material(MaterialManager::instance().mat_fire_distortion.get());
		core->set_init_color(FIRE_COLOR);
	}
	//waveの初期設定
	{
		wave = std::make_unique<MeshEffect>("./resources/Effects/Meshes/torus.fbx");
		wave->set_material(MaterialManager::instance().mat_fire_distortion.get());
		wave->set_init_color(FIRE_COLOR);
	}
	//tornadoの初期設定
	{
		tornado = std::make_unique<MeshEffect>("./resources/Effects/Meshes/eff_tornado4.fbx");
		tornado->set_material(MaterialManager::instance().mat_fire_distortion.get());
		tornado->set_init_color(FIRE_COLOR);
	}
	//黒tornadoの初期設定
	{
		tornado_black = std::make_unique<MeshEffect>("./resources/Effects/Meshes/eff_tornado4.fbx");
		tornado_black->set_material(MaterialManager::instance().mat_fire_distortion.get());
		tornado_black->set_init_color({ 0.3f,0.2f,0.0f,0.8f });
	}
	//予兆エフェクト
	{
		omen_effect = std::make_unique<MeshEffect>("./resources/Effects/Meshes/eff_aura.fbx");
		omen_effect->set_material(MaterialManager::instance().mat_fire_distortion.get());
		omen_effect->set_init_life_duration(0.5f);
		omen_effect->set_init_scale({ 0.0f, 0.0f, 11.0f });
		omen_effect->set_init_color({ FIRE_COLOR });
	}
	//定数バッファ初期設定
	constants = std::make_unique<Constants<ChargeAttackConstants>>(graphics.get_device().Get());
	//particleの初期設定
	{
		particle = std::make_unique<GPU_Particles>(graphics.get_device().Get(), 70000);
		particle.get()->initialize();
		particle.get()->set_emitter_rate(6000);
		particle.get()->set_emitter_life_time(4);
		particle.get()->set_particle_life_time(5);
		particle.get()->set_particle_size(DirectX::XMFLOAT2(0.2f, 0.2f));
		particle.get()->set_color(FIRE_COLOR);

	}
	//メテオ
	{
		const int METEORE_MAX_NUM = 12;
		meteores = std::make_unique<Meteore>(METEORE_MAX_NUM);
		create_cs_from_cso(graphics.get_device().Get(), "shaders/boss_charge_attack_emit.cso", emit_cs.ReleaseAndGetAddressOf());
		create_cs_from_cso(graphics.get_device().Get(), "shaders/boss_charge_attack_update.cso", update_cs.ReleaseAndGetAddressOf());
		meteo_span = ATTACK_TIME / (meteores->get_max_num() + 1);
		meteo_launch_radius = 20;
	}
	//トルネードの部分のダメージ設定
	{
		attack_param.power = 10;
		attack_param.invinsible_time = 0.5f;
	}
	//カメラシェイク
	{
		camera_shake.max_x_shake = 3.0f;
		camera_shake.max_y_shake = 7.0f;
		camera_shake.time = 0.5f;

	}

	const float range = 20.0f;
	boss_light = make_shared<PointLight>(position, range, DirectX::XMFLOAT3(FIRE_COLOR.x, FIRE_COLOR.y, FIRE_COLOR.z));

}
//==============================================================
// 
//発動
// 
//==============================================================
void ChargeAttack::chant(DirectX::XMFLOAT3 pos)
{
	//アクティブ状態に
	active = true;
	//未チャージ状態に
	is_charge_max = false;
	//攻撃タイマーリセット
	attack_time = 0;
	//アップデート関数をチャージに変更
	charge_attack_update = &ChargeAttack::charging_update;
	//タイマーリセット
	life_time = 0.0f;
	//コア位置設定
	position = pos;
	const float offset = 70;
	DirectX::XMFLOAT3 core_pos = { position.x, position.y + offset, position.z };
	{
		core->set_init_scale(0);
		core->play(core_pos);
		core->set_is_loop(true);
	}
	//wave初期設定
	{
		DirectX::XMFLOAT3 wave_pos = { position.x, position.y + offset, position.z };
		wave->set_init_scale(0.0f);
		wave->play(wave_pos);
		wave->constants->data.threshold = 0;
	}
	//トルネード初期化
	{
		tornado->set_init_scale(0);
		tornado->play(pos);
		//tornado->constants->data.scroll_direction = { 0.0f,-0.2f };
		tornado->constants->data.threshold = 0;
	}
	//トルネード黒初期化
	{
		tornado_black->set_init_scale(0);
		tornado_black->play(pos);
		//tornado_black->constants->data.scroll_direction = { 0.0f,-0.2f };
		tornado_black->constants->data.threshold = 0;
	}
	//予兆エフェクト初期化
	{
		omen_effect->play(pos);
	}
	//GPUパーティクル
	DirectX::XMFLOAT3 emit_pos{};
	emit_pos.y = core_pos.y;
	const float radius = 70.0f;
	for (float theta = 0; theta < 360.0f; theta += 120.0f)
	{
		emit_pos.x = core_pos.x + (radius * sinf(DirectX::XMConvertToRadians(theta)));
		emit_pos.z = core_pos.z + (radius * cosf(DirectX::XMConvertToRadians(theta)));
		particle.get()->set_emitter_pos(emit_pos);
		particle.get()->launch_emitter(emit_cs);
	}
	//サブカラー設定
	const DirectX::XMFLOAT3 sub_color = { 3.0f,0.5f,3.8f };
	constants->data.particle_sub_color = sub_color;

	//隕石
	{
		meteo_time = 0;
		meteores->initialize();
		meteo_launch_count = 0;
		//コアの場所に配置
		for (int i = 0; i < meteores->get_max_num(); i++)
		{
			meteores->set_position(core->get_position(), i);
		}
	}
	//コリジョン初期化
	{
		tornado_colider.center = pos;
		tornado_colider.radius = 0;
	}
	//コンスタントバッファのコアポジション登録
	constants->data.core_pos = core_pos;
	//ライト追加
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
void ChargeAttack::update(float elapsed_time, Camera* camera)
{
	Graphics& graphics = Graphics::instance();
	if (active)
	{
		life_time += elapsed_time;
		//更新
		(this->*charge_attack_update)(elapsed_time, camera);
		core->update(elapsed_time);
		wave->update(elapsed_time);
		tornado->update(elapsed_time);
		tornado_black->update(elapsed_time);
		omen_effect->update(elapsed_time);
	}
	meteores->update(elapsed_time);

	particle.get()->update(graphics.get_dc().Get(), elapsed_time, update_cs.Get());
}
//==============================================================
// 
//描画
// 
//==============================================================
void ChargeAttack::render(Camera* camera)
{
	Graphics& graphics = Graphics::instance();
	if (active)
	{
		core->render(camera);

		tornado->render();
		tornado_black->render();
		wave->render(camera);
		omen_effect->render(camera);
	}

	//隕石描画
	meteores->render(camera);

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
		ImGui::DragFloat3("particle_sub_color", &constants->data.particle_sub_color.x, 0.1f, 0);
		ImGui::DragFloat("meteo_span", &meteo_span, 0.1f, 0);
		ImGui::DragFloat("core_radius", &constants->data.core_radius, 1, 0);
		ImGui::DragFloat("launch_radius", &meteo_launch_radius, 1, 0);
		ImGui::DragFloat("radius", &tornado_colider.radius, 1, 0);
		if (ImGui::CollapsingHeader("camera_shake"))
		{
			ImGui::DragFloat("combo1_shake_x", &camera_shake.max_x_shake, 0.1f);
			ImGui::DragFloat("combo1_shake_y", &camera_shake.max_y_shake, 0.1f);
			ImGui::DragFloat("combo1_time", &camera_shake.time, 0.1f);
			ImGui::DragFloat("combo1_smmoth", &camera_shake.shake_smoothness, 0.1f, 0.1f, 1.0f);
		}
		ImGui::End();
	}
#endif
	core->debug_gui("core");

	wave->debug_gui("wave");
	tornado->debug_gui("tornad");
	tornado_black->debug_gui("tornado_black");
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

	if (Collision::sphere_vs_capsule(tornado_colider.center, tornado_colider.radius,
		capsule_start, capsule_end, colider_radius))
	{
		//当たり判定の位置と大きさ更新
		damaged_func(attack_param.power, attack_param.invinsible_time, WINCE_TYPE::NONE);
	}
}
//==============================================================
// 
//チャージ中のアップデート
// 
//==============================================================
void ChargeAttack::charging_update(float elapsed_time, Camera* camera)
{
	Graphics& graphics = Graphics::instance();
	//更新処理
	float core_scale = lerp(core->get_scale().x, 0.3f, 0.2f * elapsed_time);
	core->set_scale(core_scale);
	core->constants->data.scroll_speed += elapsed_time;
	core->update(elapsed_time);

	//コアの重力設定
	constants->data.core_gravitation = 0.5f;

	constants->bind(graphics.get_dc().Get(), 10, CB_FLAG::CS);

	//隕石を地面から浮き上がらせる
	for (int i = 0; i < meteores->get_max_num(); i++)
	{
		//スケール
		const float target_scale = 5.0f;
		const float scale_speed = 0.5f;
		meteores->gradual_expansion(elapsed_time, target_scale, scale_speed, i);
	}

	//予兆エフェクト
	{
		if (!omen_effect->get_active()) omen_effect->play(position);
		//拡大を繰り返して波形っぽく
		const float scale_speed = 108.0f;
		const float omen_add_scale = omen_effect->get_scale().x + scale_speed * elapsed_time;
		omen_effect->set_scale({ omen_add_scale, omen_add_scale, omen_effect->get_scale().z });
		omen_effect->rotate_base_axis(MeshEffect::AXIS::FORWARD, { 0,1,0 });
	}
	//チャージ完了時の処理
	if (is_charge_max)
	{
		core->set_scale(0.1f);
		charge_attack_update = &ChargeAttack::activities_update;
		tornado->set_scale({ 0.0f,15.0f,0.0f });
		tornado_black->set_scale({ 0.0f,15.0f,0.0f });
	}

	//カメラシェイク
	{
		camera->set_camera_shake(camera_shake);
	}
}
//==============================================================
// 
//チャージが完了し、発動したときのアップデート
// 
//==============================================================
void ChargeAttack::activities_update(float elapsed_time, Camera* camera)
{
	Graphics& graphics = Graphics::instance();
	auto fade_out = [=](float alpha) {return (std::max)(alpha - 0.5f * elapsed_time, 0.0f); };

	//コアエフェクト
	{
		const float core_s = lerp(core->get_scale().x, 10.0f, 1.5f * elapsed_time);
		core->set_scale(core_s);
		core->constants->data.scroll_speed += elapsed_time;
		core->update(elapsed_time);

		//コアの重力設定
		//反転させてパーティクルを飛び散らせる
		constants->data.core_gravitation = -0.4f;
		constants->bind(graphics.get_dc().Get(), 10, CB_FLAG::CS);
	}
	//波動エフェクト
	{
		DirectX::XMFLOAT3 wave_scale = wave->get_scale();
		wave_scale.x = lerp(wave_scale.x, 6.0f, 5.0f * elapsed_time);
		wave_scale.z = lerp(wave_scale.z, 6.0f, 5.0f * elapsed_time);
		wave->constants->data.threshold = lerp(wave->constants->data.threshold, 1.0f, 3.0f * elapsed_time);
		if (wave->get_scale().x > 5.5f)
		{
			wave_scale.x = 0.0f;
			wave_scale.z = 0.0f;
			wave->constants->data.threshold = 0;
		}
		wave->set_scale(wave_scale);
		wave->constants->data.scroll_speed += elapsed_time;
		wave->set_rotate_quaternion(MeshEffect::AXIS::UP, 360 * elapsed_time);
	}
	//竜巻エフェクト
	{
		float tornado_scale_xz = lerp(tornado->get_scale().x, 15.0f, 5.0f * elapsed_time);
		tornado->set_scale({ tornado_scale_xz, tornado->get_scale().y, tornado_scale_xz });
		tornado->set_rotate_quaternion(MeshEffect::AXIS::UP, 260 * elapsed_time);
	}
	//黒部分
	{
		float tornado_black_scale_xz = lerp(tornado_black->get_scale().x, 17.0f, 7.0f * elapsed_time);
		tornado_black->set_scale({ tornado_black_scale_xz, tornado_black->get_scale().y, tornado_black_scale_xz });
		tornado_black->set_rotate_quaternion(MeshEffect::AXIS::UP, 240 * elapsed_time);
	}
	//コライダーの半径設定
	{
		tornado_colider.radius = 50.0f;
	}

	//メテオ発生
	attack_time += elapsed_time;
	if (attack_time <= ATTACK_TIME || meteo_launch_count < meteores->get_max_num())
	{
		meteo_time += elapsed_time;
		if (meteo_time >= meteo_span)
		{
			//power
			const float min = 50;
			const float max = 100;
			const float random = Noise::instance().random_range(min, max);
			float power = random;
			//方向
			DirectX::XMFLOAT3 direction = Math::circumferential_placement(core->get_position(),
				power, meteo_launch_count, meteores->get_max_num());
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

	//ポストエフェクトのラジアルブラー実行
	{
		float rate = 0.8f;
		const float radial_power = lerp(camera->get_post_effect()->get_radial_blur_power(), 0.6f, rate * elapsed_time);
		camera->get_post_effect()->set_radial_blur_power(radial_power);

	}
	if (attack_time >= ATTACK_TIME) charge_attack_update = &ChargeAttack::vanishing_update;
}
//==============================================================
// 
//消滅時のアップデート
// 
//==============================================================
void ChargeAttack::vanishing_update(float elapsed_time, Camera* camera)
{
	//徐々に消えていく関数
	const float fade_out_speed = 7.0f;
	auto fade_out = [=](float alpha) {return (std::max)(alpha - fade_out_speed * elapsed_time, 0.0f); };

	wave->constants->data.particle_color.w = fade_out(wave->constants->data.particle_color.w);
	//wave->constants->data.scroll_speed += elapsed_time;
	const float threshold_speed = 2.0f;
	wave->constants->data.threshold = (std::min)(wave->constants->data.threshold + threshold_speed * elapsed_time, 1.0f);
	wave->set_rotate_quaternion(MeshEffect::AXIS::UP, 520 * elapsed_time);

	//トルネードの処理
	{
		float expand = 15 * elapsed_time;
		tornado->set_scale({ tornado->get_scale().x + expand,tornado->get_scale().y + expand,tornado->get_scale().z + expand });
		tornado->constants->data.particle_color.w = fade_out(tornado->constants->data.particle_color.w);
		tornado->set_rotate_quaternion(MeshEffect::AXIS::UP, 360 * elapsed_time);

		//トルネードのディゾルブ処理
		const float dissolve_rate = 2.0f;
		const float start_threshold_time = 0.5f;
		if (tornado->constants->data.threshold <= 1.0f)
		{
			tornado->constants->data.threshold += dissolve_rate * elapsed_time;
		}
	}

	//トルネードの黒部分の処理
	{
		float expand = 15 * elapsed_time;
		tornado_black->set_scale({ tornado_black->get_scale().x + expand,tornado_black->get_scale().y + expand,tornado_black->get_scale().z + expand });
		tornado_black->constants->data.particle_color.w = fade_out(tornado_black->constants->data.particle_color.w);
		tornado_black->set_rotate_quaternion(MeshEffect::AXIS::UP, 360 * elapsed_time);

		//トルネードのディゾルブ処理
		const float dissolve_rate = 1.0f;
		const float start_threshold_time = 0.5f;
		if (tornado_black->constants->data.threshold <= 1.0f)
		{
			tornado_black->constants->data.threshold += dissolve_rate * elapsed_time;
		}
	}
	//コライダーの半径設定
	{
		tornado_colider.radius = 0.0f;
	}

	//ポストエフェクトのラジアルブラー実行
	{
		float rate = 0.8f;
		const float radial_power = lerp(camera->get_post_effect()->get_radial_blur_power(), 0.0f, rate * elapsed_time);
		camera->get_post_effect()->set_radial_blur_power(radial_power);

	}

	//トルネードが消えたら終了
	if (tornado_black->constants->data.threshold > 0.9f)
	{
		//0に到達していなかったら強制的に０に
		camera->get_post_effect()->set_radial_blur_power(0);

		stop();
	}
}
