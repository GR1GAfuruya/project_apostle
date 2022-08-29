#include "boss_charge_attack.h"
#include "user.h"
ChargeAttack::ChargeAttack(Graphics& grapghics)
{
	for (int i = 0; i < 6; i++)
	{
		aura[i] = make_unique<Aura>(grapghics.get_device().Get());
	}

	core = make_unique<Sphere>(grapghics.get_device().Get());

	for (int i = 0; i < 2; i++)
	{
		prominence[i] = make_unique<Slash>(grapghics.get_device().Get());
	}
}

void ChargeAttack::play(DirectX::XMFLOAT3 pos)
{
	//�A�N�e�B�u��Ԃ�
	active = true;
	//�^�C�}�[���Z�b�g
	life_time = 0.0f;

	//�R�A�ʒu�ݒ�
	position = pos;
	DirectX::XMFLOAT3 core_pos = { position.x,position.y + 10,position.z };
	core->play(core_pos);
	core->set_scale( 0);
	core->set_is_loop(true);
	core->constants->data.particle_color = { 3.0f,0.6f,0.0f,0.8f };
	core->constants->data.scroll_direction = { 1.0f,-1.0f };
	//�R�A�։��т�G�t�F�N�g�̃v���C���̏����@
	//�{�X�����3�p�`�̈ʒu
	DirectX::XMFLOAT3 pillar_point[3];
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

		//�x���̈ʒu�ݒ�A�n�ʂ���R�A�̌����ɌX����
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
		//2�{�̂�����1�̊p�x�����炵�ė������ۂ�
		if (i == 0 || i == 1 || i == 2)
		{
			aura[i]->set_orientation(Math::rot_quaternion(aura[i]->get_orientation(), Math::get_posture_forward(aura[i]->get_orientation()), DirectX::XMConvertToRadians(180)));
		}
		aura[i]->aura_constants->data.particle_color = { 2.0f,0.6f,0.0f,0.5f };
		aura[i]->aura_constants->data.scroll_direction.y = 1.5f;
		aura[i]->rot_speed = -520;
		aura[i]->set_scale({ 1.0f, 1.0f, 3.0f });

		//�R�A����̉�
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
		//�X�V����
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
		//��������
		if (life_time > life_span)
		{
			if (is_loop)
			{
				life_time = 0;
			}
			else
			{
				stop();
			}
		}
	}
}

void ChargeAttack::render(Graphics& graphics)
{
#if USE_IMGUI
	imgui_menu_bar("Effects", "boss_charge", display_imgui);
	ImGui::Begin("boss_charge");
	ImGui::DragFloat3("position", &position.x, 0.1);
	ImGui::DragFloat3("scale", &scale.x, 0.1);
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
}
