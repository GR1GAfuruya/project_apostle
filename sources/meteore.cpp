#include "meteore.h"
#include "stage_manager.h"
#include "material_manager.h"
//==============================================================
// 
// �R���X�g���N�^
// 
//==============================================================
Meteore::Meteore(Graphics& graphics, int max_num)
{
	main_effect = make_unique<InstanceMeshEffect>(graphics, "./resources/Effects/Meshes/meteore3.fbx", max_num);
	main_effect->set_material(MaterialManager::instance().mat_meteore.get());
	main_effect->constants->data.particle_color = { 4.0f, 1.0f, 0.7f, 0.8f };
	
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
	}
}
//==============================================================
// 
//�X�V����
// 
//==============================================================
void Meteore::update(Graphics& graphics, float elapsed_time)
{
	main_effect->update(graphics,elapsed_time);


	for (int i = 0; i < MAX_NUM; i++)
	{
		main_effect->set_position(params[i].position, i);
		main_effect->set_scale(params[i].scale, i);
		//���x�v�Z�����邩�ǂ���
		if (params[i].is_calc_velocity)
		{
			update_velocity(elapsed_time,i);
		};

		//���Ńq�b�g�����ꍇ�̓T�C�Y���O�ɂ���
		if (params[i].is_hit)
		{
			params[i].scale = Math::lerp(params[i].scale, { 0,0,0 }, 1.0f * elapsed_time);
			main_effect->set_scale(params[i].scale, i);
		}
	}
	
}
//==============================================================
// 
//�`�揈���i�f�B�t�@�[�h�j
// 
//==============================================================
void Meteore::render(Graphics& graphics)
{
	main_effect->render(graphics);
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
//�w��̈ʒu�ɏ㏸
// 
//==============================================================
void Meteore::rising(float elapsed_time, DirectX::XMFLOAT3 target_position, float target_scale, float rise_speed, int index)
{
	params[index].position = Math::lerp(params[index].position, target_position, rise_speed * elapsed_time);
	params[index].scale = Math::lerp(params[index].scale, { target_scale,target_scale,target_scale }, rise_speed * elapsed_time);
	DirectX::XMFLOAT3 s = params[index].scale;
	int a;
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
	//params[index].scale = { 0,0,0 };
}
