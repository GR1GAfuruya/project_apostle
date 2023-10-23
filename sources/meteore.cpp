#include "meteore.h"
#include "stage_manager.h"
#include "material_manager.h"
#include "Operators.h"
//==============================================================
// 
// �R���X�g���N�^
// 
//==============================================================
Meteore::Meteore(int max_num)
{
	Graphics& graphics = Graphics::instance();
	main_effect = make_unique<InstanceMeshEffect>("./resources/Effects/Meshes/meteore3.fbx", max_num);
	main_effect->set_material(MaterialManager::instance().mat_meteore.get());
	main_effect->constants->data.particle_color = { 4.0f, 1.0f, 0.7f, 0.8f };

	for (int i = 0; i < max_num; i++)
	{
		std::unique_ptr<MeshEffect> m_wave = make_unique<MeshEffect>("./resources/Effects/Meshes/eff_sphere.fbx");
		m_wave->set_material(MaterialManager::instance().mat_fire_distortion.get());
		m_wave->set_init_color({ 4.0f, 1.0f, 0.7f, 0.8f });
		m_wave->set_init_scale(0);
		m_wave->set_init_life_duration(1.5f);

		meteo_wave.push_back(std::move(m_wave));
	}

	MAX_NUM = max_num;
	//�p�����[�^�[������
	initialize();
}

//==============================================================
// 
// ������
// 
//==============================================================
void Meteore::initialize()
{
	acceleration = 15.0f;
	friction = 0.0f;
	max_move_speed = 40.0f;
	main_effect->stop();
	main_effect->play({ 0,0,0 });
	main_effect->set_is_loop(true);

	//�U���͐ݒ�
	power = 100;
	invinsible_time = 2.0f;
	//覐΂��Ƃ̃p�����[�^�[������
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
//�X�V����
// 
//==============================================================

void Meteore::update(float elapsed_time)
{
	main_effect->update(elapsed_time);
	//meteo_wave->update(graphics,elapsed_time);
	//���a
	radius = 3.5f;
	for (int i = 0; i < MAX_NUM; i++)
	{
		main_effect->set_position(params[i].position, i);
		main_effect->set_scale(params[i].scale, i);
		//���x�v�Z�����邩�ǂ���
		if (params[i].is_calc_velocity)
		{
			update_velocity(elapsed_time, i);
			DirectX::XMFLOAT3 aura_dir = { -params[i].velocity.x,-params[i].velocity.y ,-params[i].velocity.z };
			DirectX::XMFLOAT3 add_pos = Math::vector_scale(Math::Normalize(params[i].velocity), 5);
		};

		//�q�b�g�����ꍇ�̓T�C�Y���O�ɂ���
		if (params[i].is_hit)
		{
			params[i].scale = { 0,0,0 };
			main_effect->set_scale(params[i].scale, i);

			{
				const float add_scale_rate = 10.0f;
				const float target_scale = 0.2f;
				float add_scale = lerp(meteo_wave[i]->get_scale().x, target_scale, add_scale_rate * elapsed_time);
				//add_scale = (std::min)(add_scale, target_scale);
				meteo_wave[i]->set_scale(add_scale);
			}
			//
			{
				DirectX::XMFLOAT3 wave_target_color = { 0,0,0 };
				DirectX::XMFLOAT3 wave_now_color = { meteo_wave[i]->get_color().x,meteo_wave[i]->get_color().y,meteo_wave[i]->get_color().z };
				DirectX::XMFLOAT3 wave_color = Math::lerp(wave_now_color, wave_target_color, 1.0f * elapsed_time);
				meteo_wave[i]->set_color(DirectX::XMFLOAT4(wave_color.x, wave_color.y, wave_color.z, 0.5f));
			}
		}

		meteo_wave.at(i)->update(elapsed_time);
		//�����蔻��̈ʒu�Ƒ傫���X�V
		params[i].colider_sphere.center = params[i].position;
		params[i].colider_sphere.radius = params[i].scale.x * radius;
	}

}
//==============================================================
// 
//�`�揈���i�f�B�t�@�[�h�j
// 
//==============================================================
void Meteore::render(Camera* camera)
{
	main_effect->render();
	for (auto& m : meteo_wave)
	{
		if (m->get_active())
		{
			m->render(camera);
		}
	}
}

void Meteore::debug_gui()
{
#if  USE_IMGUI
	ImGui::Begin("meteore");
	for (int i = 0; i < MAX_NUM; i++)
	{
		meteo_wave[i]->debug_gui("meteo_wave" + to_string(i));
	}
	ImGui::End();
#endif //  USE_IMGUI

}

//==============================================================
// 
//�~�����覐΂�z�u
// 
//==============================================================
void Meteore::create_on_circle(DirectX::XMFLOAT3 center, float radius, int index)
{
	//覐΂��~����ɔz�u
	params[index].position.x = Math::circumferential_placement({ center.x,center.z }, radius, index, MAX_NUM).x;
	params[index].position.y = center.y;
	params[index].position.z = Math::circumferential_placement({ center.x,center.z }, radius, index, MAX_NUM).y;
};

//==============================================================
// 
//���X�ɑ傫���Ȃ�
// 
//==============================================================
void Meteore::gradual_expansion(float elapsed_time, float target_scale, float scale_speed, int index)
{
	//���X�ɑ傫��
	params[index].scale = Math::lerp(params[index].scale, { target_scale,target_scale,target_scale }, scale_speed * elapsed_time);
	//�����蔻��̈ʒu�Ƒ傫���X�V
	params[index].colider_sphere.center = params[index].position;
	params[index].colider_sphere.radius = params[index].scale.x;
}
//==============================================================
// 
//�������
// 
//==============================================================
void Meteore::launch(DirectX::XMFLOAT3 init_vec, float speed, int index)
{
	//���łɎˏo��ԂȂ疳��
	if (params[index].is_calc_velocity) return;
	//���x�v�Z�J�n
	params[index].is_calc_velocity = true;
	params[index].is_hit = false;

	//�����蔻��̈ʒu�Ƒ傫���X�V
	params[index].colider_sphere.center = params[index].position;
	params[index].colider_sphere.radius = params[index].scale.x * radius;


	meteo_wave.at(index)->set_scale(0);

	//���˕����ݒ�
	move(init_vec.x, init_vec.z, speed, index);
}
//==============================================================
// 
//���ׂĈ�C�ɔ���
// 
//==============================================================
void Meteore::all_launch(DirectX::XMFLOAT3 init_vec, float speed)
{
	for (int i = 0; i < MAX_NUM; i++)
	{
		params[i].is_calc_velocity = true;
		params[i].is_hit = false;
		main_effect->set_scale(0, i);
		move(init_vec.x, init_vec.z, speed, i);
	}
}
//==============================================================
// 
//���x�X�V
// 
//==============================================================
void Meteore::update_velocity(float elapsed_time, int index)
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
void Meteore::calc_meteore_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func)
{
	for (int i = 0; i < MAX_NUM; i++)
	{
		if (!params[i].is_hit)
		{
			if (Collision::sphere_vs_capsule(params[i].colider_sphere.center, params[i].colider_sphere.radius,
				capsule_start, capsule_end, colider_radius))
			{
				//�����蔻��̈ʒu�Ƒ傫���X�V
				damaged_func(power, invinsible_time, WINCE_TYPE::SMALL);
				params[i].scale = { 0,0,0 };
				on_hit(i);
			}
		}
	}

}
//==============================================================
// 
//�ړ������̐ݒ�
// 
//==============================================================
void Meteore::move(float vx, float vz, float speed, int index)
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
void Meteore::update_vertical_velocity(float elapsed_frame, int index)
{
	params[index].velocity.y += gravity * elapsed_frame;
}

void Meteore::update_vertical_move(float elapsed_time, int index)
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


			//TODO:�����������̏����Ɉڍs
			on_hit(index);
			//main_effect->stop();
			//�������~�߂�
			params[index].velocity = { 0,0,0 };
			params[index].move_vec.x = 0.0f;
			params[index].move_vec.z = 0.0f;

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

void Meteore::update_hrizontal_velocity(float elapsed_frame, int index)
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

void Meteore::update_horizontal_move(float elapsed_time, int index)
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
		DirectX::XMFLOAT3 start = { params[index].position.x - mx / 50.0f, params[index].position.y * 2, params[index].position.z - mz / 50.0f };
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

void Meteore::on_hit(int index)
{
	params[index].is_calc_velocity = false;
	params[index].is_hit = true;

	meteo_wave[index]->play(params[index].position);
	meteo_wave[index]->rot_speed.y = 60.0f;
}
