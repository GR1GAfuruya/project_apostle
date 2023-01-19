#include "boss_attack_skill1.h"
#include "stage_manager.h"
//==============================================================
// 
//�R���X�g���N�^
// 
//==============================================================
BossAttackSkill1::BossAttackSkill1(Graphics& graphics)
{
	acceleration = 15.0f;
	friction = 0.0f;
	max_move_speed = 40.0f;

	at_param.power = 100;
	at_param.invinsible_time = 2.0f;

	range = 10;
	
	main_effect = make_unique<InstanceMeshEffect>(graphics, "./resources/Effects/Meshes/meteore3.fbx", MAX_NUM);
	main_effect->set_material(MaterialManager::instance().mat_meteore.get());
	main_effect->constants->data.particle_color = { 4.0f, 1.0f, 0.7f, 0.8f };

	for (auto& m : meteo_wave)
	{
		m = make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_sphere.fbx");
		m->set_material(MaterialManager::instance().mat_fire_distortion.get());
		m->set_init_color({ 4.0f, 1.0f, 0.7f, 0.8f });
		m->set_init_life_duration(2.0f);
		m->set_init_scale(0);
	}

	state_update = [=](Graphics& graphics, float elapsed_time, Camera* camera)
		->void {return attack_state_update(graphics, elapsed_time, camera); };

	main_effect->play({ 0,0,0 });
	main_effect->set_is_loop(true);
	params.reset(new MeteoreParam[MAX_NUM]);
	for (int i = 0; i < MAX_NUM; i++)
	{
		params[i].position = { 0,0,0 };
		params[i].velocity = { 0,0,0 };
		params[i].scale = { 0,0,0 };
		params[i].is_calc_velocity = false;
		params[i].is_hit = false;
		main_effect->set_scale(0, i);
		main_effect->set_position({ 0,0,0 }, i);
		params[i].colider_sphere.radius = params[i].scale.x;
	}
}

//==============================================================
// 
//����
// 
//==============================================================
void BossAttackSkill1::chant(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir)
{
	state_update = [=](Graphics& graphics, float elapsed_time, Camera* camera)
		->void {return charge_state_update(graphics, elapsed_time, camera); };

	for (int i = 0; i < MAX_NUM; i++)
	{
		const float angle = 45.0f;
		float theta = angle * (i - 1);
		//theta = DirectX::XMConvertToRadians(theta);
		skill_dir[i].x = dir.z * sinf(theta) + dir.x * cosf(theta);
		skill_dir[i].z = dir.z * cosf(theta) - dir.x * sinf(theta);

		move(skill_dir[i].x, skill_dir[i].z, 50, i);
		
		params[i].position = pos;
		params[i].is_calc_velocity = true;
		params[i].is_hit = false;
		main_effect->set_scale(0, i);
		meteo_wave[i]->set_scale(0);
		params[i].scale = { 2,2,2 };
	}
}

//==============================================================
// 
//��~
// 
//==============================================================
void BossAttackSkill1::stop()
{
}
//==============================================================
// 
//�X�V
// 
//==============================================================
void BossAttackSkill1::update(Graphics& graphics, float elapsed_time, Camera* camera)
{
	state_update(graphics, elapsed_time, camera);

	main_effect->update(graphics, elapsed_time);

	for (auto& m : meteo_wave)
	{
		if (m->get_active())
		{
			m->update(graphics, elapsed_time);
		}
	}
};



//==============================================================
// 
//�`���[�W���Ă���Ƃ��̍X�V
// 
//==============================================================
void BossAttackSkill1::charge_state_update(Graphics& graphics, float elapsed_time, Camera* camera)
{
	state_update = [=](Graphics& graphics, float elapsed_time, Camera* camera)
		->void {return attack_state_update(graphics, elapsed_time, camera); };

}

//==============================================================
// 
//�U�������Ƃ��̍X�V
// 
//==============================================================
void BossAttackSkill1::attack_state_update(Graphics& graphics, float elapsed_time, Camera* camera)
{
	for (int i = 0; i < MAX_NUM; i++)
	{
		main_effect->set_position(params[i].position, i);
		main_effect->set_scale(params[i].scale, i);
		//���x�v�Z�����邩�ǂ���
		if (params[i].is_calc_velocity)
		{
			update_velocity(elapsed_time, i);
			
		};

		//�q�b�g�����ꍇ�̓T�C�Y���O�ɂ���
		if (params[i].is_hit)
		{
			params[i].scale = { 0,0,0 };
			main_effect->set_scale(params[i].scale, i);

			//覐΂̔j�􉉏o
			{
				params[i].scale = { 0,0,0 };
				main_effect->set_scale(params[i].scale, i);

				float add_scale = lerp(meteo_wave[i]->get_scale().x, 0.2f, 1.0f * elapsed_time);
				add_scale = (std::min)(add_scale, 0.2f);
				meteo_wave[i]->set_scale(add_scale);
				DirectX::XMFLOAT3 wave_target_color = { 0,0,0 };
				DirectX::XMFLOAT3 wave_now_color = { meteo_wave[i]->get_color().x,meteo_wave[i]->get_color().y,meteo_wave[i]->get_color().z };
				DirectX::XMFLOAT3 wave_color = Math::lerp(wave_now_color, wave_target_color, 0.1f * elapsed_time);
				meteo_wave[i]->set_color(DirectX::XMFLOAT4(wave_color.x, wave_color.y, wave_color.z, 0.5f));
			}
		}
		meteo_wave[i]->update(graphics, elapsed_time);
		//�����蔻��̈ʒu�Ƒ傫���X�V
		params[i].colider_sphere.center = params[i].position;
		params[i].colider_sphere.radius = params[i].scale.x * 3.5f;
	}
}

//==============================================================
// 
//�`��
// 
//==============================================================
void BossAttackSkill1::render(Graphics& graphics)
{
	main_effect->render(graphics);
	for (int i = 0; i < MAX_NUM; i++)
	{
		meteo_wave[i]->render(graphics);
	}
}

//==============================================================
// 
//�f�o�b�OGUI
// 
//==============================================================
void BossAttackSkill1::debug_gui(const char* str_id)
{
#if  USE_IMGUI
	for (int i = 0; i < MAX_NUM; i++)
	{
		meteo_wave[i]->debug_gui("skill1_wave" + to_string(i));
	}
#endif //  USE_IMGUI

}

void BossAttackSkill1::calc_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func)
{
	for (int i = 0; i < MAX_NUM; i++)
	{
		if (!params[i].is_hit)
		{
			if (Collision::sphere_vs_capsule(params[i].colider_sphere.center, params[i].colider_sphere.radius,
				capsule_start, capsule_end, colider_radius))
			{
				//�����蔻��̈ʒu�Ƒ傫���X�V
				damaged_func(at_param.power, at_param.invinsible_time, WINCE_TYPE::SMALL);
				on_hit(i);
			}
		}
	}
}

//==============================================================
// 
//���x�X�V
// 
//==============================================================
void BossAttackSkill1::update_velocity(float elapsed_time, int index)
{
	//�o�߃t���[��
	float elapsed_frame = 60.0f * elapsed_time;


	//�������͍X�V����
	update_vertical_velocity(elapsed_frame, index);

	//�����ړ��X�V����
	update_vertical_move(elapsed_time, index);

	//�������͍X�V����
	update_hrizontal_velocity(elapsed_frame, index);

	//�����ړ��X�V����
	update_horizontal_move(elapsed_time, index);

}
//==============================================================
// 
//�ړ������̐ݒ�
// 
//==============================================================
void BossAttackSkill1::move(float vx, float vz, float speed, int index)
{
	//�����ݒ�
	params[index].move_vec.x = vx;
	params[index].move_vec.z = vz;

	//�ő呬�x�ݒ�
	max_move_speed = speed;
}
//==============================================================
// 
//����������
// 
//==============================================================
void BossAttackSkill1::update_vertical_velocity(float elapsed_frame, int index)
{
	params[index].velocity.y += -1.0f * elapsed_frame;
}

void BossAttackSkill1::update_vertical_move(float elapsed_time, int index)
{
	// �L�����N�^�[�̉������̈ړ���
	float my = params[index].velocity.y * elapsed_time;


	// �L�����N�^�[��Y�������ƂȂ�@���x�N�g��
	// ������
	if (my < 0.0f)
	{
		//���C�̊J�n�ʒu�͏�����
		DirectX::XMFLOAT3 start = { params[index].position.x, params[index].position.y + 0.5f, params[index].position.z };
		//���C�̏I�_�ʒu�͈ړ���̈ʒu
		DirectX::XMFLOAT3 end = { params[index].position.x, params[index].position.y + my * ray_power  , params[index].position.z };

		//���C�L���X�g�ɂ��n�ʔ���
		HitResult hit;
		if (StageManager::Instance().ray_cast(start, end, hit))
		{
			//�n�ʂɐݒu���Ă���
			params[index].position = hit.position;
		}
		else
		{
			//�󒆂ɕ����Ă���
			params[index].position.y += my;
		}

	}
	// �㏸��
	else if (my > 0.0f)
	{
		params[index].position.y += my;
	}

}

void BossAttackSkill1::update_hrizontal_velocity(float elapsed_frame, int index)
{
	//XZ���ʂ̑���
	float length = sqrtf(params[index].velocity.x * params[index].velocity.x + params[index].velocity.z * params[index].velocity.z);

	//XZ���ʂ̑��͂���������
	if (length <= max_move_speed)
	{
		//�ړ��x�N�g�����[���x�N�g���łȂ��Ȃ��������
		float moveVecLength = sqrtf(params[index].move_vec.x * params[index].move_vec.x + params[index].move_vec.z * params[index].move_vec.z);


		if (moveVecLength > 0.0f)
		{
			//������
			float acceleration = this->acceleration * elapsed_frame;
			//�󒆂ɂ���Ƃ��͉����͂����炷
			//�ړ��x�N�g���ɂ���������
			params[index].velocity.x += params[index].move_vec.x * acceleration;
			params[index].velocity.z += params[index].move_vec.z * acceleration;

			//�ő呬�x����
			float length = sqrtf(params[index].velocity.x * params[index].velocity.x + params[index].velocity.z * params[index].velocity.z);
			if (length > max_move_speed)
			{
				float vx = params[index].velocity.x / length;
				float vz = params[index].velocity.z / length;
				params[index].velocity.x = vx * max_move_speed;
				params[index].velocity.z = vz * max_move_speed;
			}
		}
	}
	else
	{
		params[index].move_vec.x = 0.0f;
		params[index].move_vec.z = 0.0f;
	}
}

void BossAttackSkill1::update_horizontal_move(float elapsed_time, int index)
{
	// �������͌v�Z
	float velocity_length_xz = sqrtf(params[index].velocity.x * params[index].velocity.x + params[index].velocity.z * params[index].velocity.z);
	// �������͂������
	if (velocity_length_xz > 0.0f)
	{

		// �����ړ��l
		float mx = params[index].velocity.x * elapsed_time;
		float mz = params[index].velocity.z * elapsed_time;

		// ���C�̊J�n�ʒu�ƏI�_�ʒu
		DirectX::XMFLOAT3 start = { params[index].position.x - mx / 50.0f, params[index].position.y + 0.5f, params[index].position.z - mz / 50.0f };
		DirectX::XMFLOAT3 end = { params[index].position.x + mx * ray_power, start.y, params[index].position.z + mz * ray_power };
		HitResult hit;
		if (StageManager::Instance().ray_cast(start, end, hit))//�����ǂ������
		{

			//TODO:�����������̏����Ɉڍs
			on_hit(index);

			params[index].position.x = hit.position.x;
			params[index].position.z = hit.position.z;
			params[index].velocity.x = 0;
			params[index].velocity.z = 0;
		}
		else
		{
			//�ړ�
			params[index].position.x += mx;
			params[index].position.z += mz;
		}

	}
}

void BossAttackSkill1::on_hit(int index)
{
	params[index].is_calc_velocity = false;
	params[index].is_hit = true;
	//params[index].scale = { 0,0,0 };
	meteo_wave[index]->play(params[index].position);
	meteo_wave[index]->rot_speed.y = 60.0f;

}

