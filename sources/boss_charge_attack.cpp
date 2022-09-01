#include "boss_charge_attack.h"
#include "user.h"
ChargeAttack::ChargeAttack(Graphics& graphics)
{
	for (int i = 0; i < 6; i++)
	{
		aura[i] = make_unique<Aura>(graphics.get_device().Get());
	}

	core = make_unique<Sphere>(graphics.get_device().Get());

	for (int i = 0; i < 2; i++)
	{
		prominence[i] = make_unique<Slash>(graphics.get_device().Get());
	}
	particle = std::make_unique<GPU_Particles>(graphics.get_device().Get());
	constants = std::make_unique<Constants<ChargeAttackConstants>>(graphics.get_device().Get());

	particle.get()->initialize(graphics.get_dc().Get());
	create_cs_from_cso(graphics.get_device().Get(), "shaders/boss_charge_attack_emit.cso", emit_cs.ReleaseAndGetAddressOf());
	create_cs_from_cso(graphics.get_device().Get(), "shaders/boss_charge_attack_update.cso", update_cs.ReleaseAndGetAddressOf());
}

void ChargeAttack::play(DirectX::XMFLOAT3 pos)
{
	//アクティブ状態に
	active = true;
	//タイマーリセット
	life_time = 0.0f;
	//コア位置設定
	position = pos;
	DirectX::XMFLOAT3 core_pos = { position.x,position.y + 10,position.z };
	core->play(core_pos);
	core->set_scale( 0);
	core->set_is_loop(true);
	core->constants->data.particle_color = { 3.0f,0.6f,0.0f,0.8f };
	core->constants->data.scroll_direction = { 1.0f,-1.0f };
	//コアへ延びるエフェクトのプレイ時の処理　
	//ボス周りに3角形の位置
	DirectX::XMFLOAT3 pillar_point[3]{};
	pillar_point[0] = { position.x - 10,position.y - 10,position.z - 10 };
	pillar_point[1] = { position.x + 10,position.y - 10,position.z - 10 };
	pillar_point[2] = { position.x, position.y - 10, position.z + 10 };
	for (int i = 0; i < 6; i++)
	{
		aura[i]->play(position);
		aura[i]->set_is_loop(true);

		DirectX::XMFLOAT4 aura_q = aura[i]->get_orientation();
		DirectX::XMFLOAT3 aura_forward = Math::get_posture_forward(aura_q);
		DirectX::XMFLOAT3 vec;

		//支柱の位置設定、地面からコアの向きに傾ける
		switch (i % 3)
		{
		case 0:
			aura[i]->set_position(pillar_point[0]);
			vec = Math::calc_vector_AtoB_normalize(position, pillar_point[0]);
			aura[i]->set_orientation(Math::rot_quaternion_dir(aura_q, aura_forward, vec));
			break;
		case 1:
			aura[i]->set_position(pillar_point[1]);
			vec = Math::calc_vector_AtoB_normalize(position, pillar_point[1]);
			aura[i]->set_orientation(Math::rot_quaternion_dir(aura_q, aura_forward, vec));
			break;
		case 2:
			aura[i]->set_position(pillar_point[2]);
			vec = Math::calc_vector_AtoB_normalize(position, pillar_point[2]);
			aura[i]->set_orientation(Math::rot_quaternion_dir(aura_q, aura_forward, vec));
			break;
		default:
			break;
		}
		//2本のうちの1つの角度をずらして螺旋っぽく
		if (i == 0 || i == 1 || i == 2)
		{
			aura[i]->set_orientation(Math::rot_quaternion(aura[i]->get_orientation(), Math::get_posture_forward(aura[i]->get_orientation()), DirectX::XMConvertToRadians(180)));
		}
		aura[i]->aura_constants->data.particle_color = { 2.0f,0.6f,0.0f,0.5f };
		aura[i]->aura_constants->data.scroll_direction.y = 1.5f;
		aura[i]->rot_speed = -520;
		aura[i]->set_scale({ 1.0f, 1.0f, 3.0f });

		//コア周りの炎
		for (int i = 0; i < 2; i++)
		{
			prominence[i]->play(core_pos);
			prominence[i]->set_is_loop(true);
			prominence[i]->set_scale(0.3f);
			prominence[i]->constance->data.particle_color = { 2.0f,0.8f,0.0f,0.6f };
			prominence[i]->set_rotate_rate(5);

		}
		prominence[0]->set_rotate_quaternion(AXIS::RIGHT, DirectX::XMConvertToRadians(90));
		prominence[1]->set_rotate_quaternion(AXIS::RIGHT, DirectX::XMConvertToRadians(-45));
		
	}
	DirectX::XMFLOAT3 emit_pos{};
	emit_pos.y = core_pos.y;
	const float radius = 50.0f;
	for (float theta = 0; theta < 360; theta += 60)
	{
		emit_pos.x = core_pos.x + (radius * sinf(DirectX::XMConvertToRadians(theta)));
		emit_pos.z = core_pos.z + (radius * cosf(DirectX::XMConvertToRadians(theta)));
		particle.get()->set_emitter_pos(emit_pos);
		particle.get()->set_emitter_rate(32);
		particle.get()->set_emitter_life_time(7);
		particle.get()->set_particle_life_time(life_time);
		particle.get()->set_particle_size(DirectX::XMFLOAT2(0.2,0.2));
		particle.get()->particle_constants->data.particle_color = { 3.0f,0.5f,0.0f,0.8f };
		particle.get()->launch_emitter(emit_cs);
	}
	
	constants->data.core_pos = core_pos;
}

void ChargeAttack::stop()
{
	for (int i = 0; i < 6; i++)
	{
		aura[i]->stop();
	}
	core->stop();

	for (int i = 0; i < 2; i++)
	{
		prominence[i]->stop();
	}
}

void ChargeAttack::update(Graphics& graphics, float elapsed_time)
{
	if (active)
	{
		//更新処理
		float core_s = lerp(core->get_scale().x, 0.3f, 0.2f * elapsed_time);
		core->set_scale(core_s);
		core->constants->data.scroll_speed += elapsed_time;
		core->update(graphics, elapsed_time);
		for (int i = 0; i < 6; i++)
		{
			aura[i]->update(graphics, elapsed_time);
			aura[i]->aura_constants->data.scroll_speed += elapsed_time;
		}
		for (int i = 0; i < 2; i++)
		{
			prominence[i]->update(graphics, elapsed_time);
			
			prominence[i]->set_scale(core_s * 2);
		}
		prominence[0]->set_rotate_quaternion(AXIS::FORWARD, DirectX::XMConvertToRadians(95));
		prominence[0]->set_rotate_quaternion(AXIS::RIGHT, DirectX::XMConvertToRadians(15));
		prominence[1]->set_rotate_quaternion(AXIS::FORWARD, DirectX::XMConvertToRadians(-95));
		prominence[1]->set_rotate_quaternion(AXIS::RIGHT, DirectX::XMConvertToRadians(-15));
		//寿命処理
		if (life_time > life_span)
		{
			if (is_loop)
			{
				play(position);
				life_time = 0;
			}
			else
			{
				stop();
			}
		}
	}
	//constants->data.core_gravitation = 0.5f;
	//constants->data.core_radius = 0.5f;
	constants->bind(graphics.get_dc().Get(), 10, CB_FLAG::CS);
	particle.get()->update(graphics.get_dc().Get(), elapsed_time, update_cs.Get());

}

void ChargeAttack::render(Graphics& graphics)
{
#if USE_IMGUI
	imgui_menu_bar("Effects", "boss_charge", display_imgui);
	ImGui::Begin("boss_charge");
	ImGui::DragFloat3("position", &position.x, 0.1);
	ImGui::DragFloat3("scale", &scale.x, 0.1);
	ImGui::DragFloat("core_gravitation", &constants->data.core_gravitation,0.1,0);
	ImGui::DragFloat("core_radius", &constants->data.core_radius,1,0);
	ImGui::Checkbox("play", &active);
	ImGui::End();
#endif
	for (int i = 0; i < 6; i++)
	{
		aura[i]->render(graphics);
	}
	core->render(graphics);

	for (int i = 0; i < 2; i++)
	{
		prominence[i]->render(graphics);
	}
	particle->render(graphics.get_dc().Get(), graphics.get_device().Get());

}
