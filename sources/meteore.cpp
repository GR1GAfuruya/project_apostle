#include "meteore.h"
#include "stage_manager.h"

//==============================================================
// 
// �R���X�g���N�^
// 
//==============================================================
Meteore::Meteore(Graphics& graphics)
{
	main_effect = make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/meteore3.fbx");
	main_effect->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill2_output.png");
	main_effect->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/Mask/dissolve_animation.png");
	main_effect->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga");
	main_effect->create_pixel_shader(graphics.get_device().Get(), "./shaders/fire_distortion.cso");
	main_effect->constants->data.particle_color = { 4.0f, 1.0f, 0.7f, 0.8f };


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
}
//==============================================================
// 
//�X�V����
// 
//==============================================================
void Meteore::update(Graphics& graphics, float elapsed_time)
{
	main_effect->set_position(position);
	main_effect->update(graphics,elapsed_time);

	if (!is_hit)
	{
		update_velocity(elapsed_time, position);
	};
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

void Meteore::launch(DirectX::XMFLOAT3 init_pos, DirectX::XMFLOAT3 init_vec, float speed)
{
	main_effect->play(init_pos);
	main_effect->set_scale(3.0f);
	//�����ʒu�ݒ�
	position = init_pos;
	velocity = { 0,0,0 };
	//���˕����ݒ�
	move(init_vec.x, init_vec.z, speed);
}

void Meteore::update_velocity(float elapsed_time, DirectX::XMFLOAT3& position)
{
	//�o�߃t���[��
	float elapsed_frame = 60.0f * elapsed_time;


	//�������͍X�V����
	update_vertical_velocity(elapsed_frame);

	//�����ړ��X�V����
	update_vertical_move(elapsed_time, position);

	//�������͍X�V����
	update_hrizontal_velocity(elapsed_frame);

	//�����ړ��X�V����
	update_horizontal_move(elapsed_time, position);

}

void Meteore::move(float vx, float vz, float speed)
{
	//�����ݒ�
	move_vec_x = vx;
	move_vec_z = vz;

	//�ő呬�x�ݒ�
	max_move_speed = speed;
}

void Meteore::update_vertical_velocity(float elapsed_frame)
{
	velocity.y += gravity * elapsed_frame;

}

void Meteore::update_vertical_move(float elapsed_time, DirectX::XMFLOAT3& position)
{
	// �L�����N�^�[�̉������̈ړ���
	float my = velocity.y * elapsed_time;


	// �L�����N�^�[��Y�������ƂȂ�@���x�N�g��
	// ������
	if (my < 0.0f)
	{
		//���C�̊J�n�ʒu�͏�����
		DirectX::XMFLOAT3 start = { position.x, position.y + 0.5f, position.z };
		//���C�̏I�_�ʒu�͈ړ���̈ʒu
		DirectX::XMFLOAT3 end = { position.x, position.y + my * ray_power  , position.z };

		//���C�L���X�g�ɂ��n�ʔ���
		HitResult hit;
		if (StageManager::Instance().ray_cast(start, end, hit))
		{
			//�n�ʂɐݒu���Ă���
			position = hit.position;

		
			//TODO:�����������̏����Ɉڍs
			if (!is_hit)
			{
				on_hit();
			}
			//main_effect->stop();
			//�������~�߂�
			velocity = { 0,0,0 };
			move_vec_x = 0.0f;
			move_vec_z = 0.0f;
			
		}
		else
		{
			//�󒆂ɕ����Ă���
			position.y += my;
		}

	}
	// �㏸��
	else if (my > 0.0f)
	{
		position.y += my;
	}

}

void Meteore::update_hrizontal_velocity(float elapsed_frame)
{
	//XZ���ʂ̑���
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

	//XZ���ʂ̑��͂���������
	if (length <= max_move_speed)
	{
		//�ړ��x�N�g�����[���x�N�g���łȂ��Ȃ��������
		float moveVecLength = sqrtf(move_vec_x * move_vec_x + move_vec_z * move_vec_z);


		if (moveVecLength > 0.0f)
		{
			//������
			float acceleration = this->acceleration * elapsed_frame;
			//�󒆂ɂ���Ƃ��͉����͂����炷
			//�ړ��x�N�g���ɂ���������
			velocity.x += move_vec_x * acceleration;
			velocity.z += move_vec_z * acceleration;

			//�ő呬�x����
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > max_move_speed)
			{
				float vx = velocity.x / length;
				float vz = velocity.z / length;
				velocity.x = vx * max_move_speed;
				velocity.z = vz * max_move_speed;
			}
		}
	}
	else
	{
		move_vec_x = 0.0f;
		move_vec_z = 0.0f;
	}
}

void Meteore::update_horizontal_move(float elapsed_time, DirectX::XMFLOAT3& position)
{
	// �������͌v�Z
	float velocity_length_xz = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	// �������͂������
	if (velocity_length_xz > 0.0f)
	{

		// �����ړ��l
		float mx = velocity.x * elapsed_time;
		float mz = velocity.z * elapsed_time;

		// ���C�̊J�n�ʒu�ƏI�_�ʒu
		DirectX::XMFLOAT3 start = { position.x - mx / 50.0f, position.y * 2, position.z - mz / 50.0f };
		DirectX::XMFLOAT3 end = { position.x + mx * ray_power, start.y, position.z + mz * ray_power };
		HitResult hit;
		if (StageManager::Instance().ray_cast(start, end, hit))//�����ǂ������
		{

			//TODO:�����������̏����Ɉڍs
			if (!is_hit)
			{
				on_hit();
			}

			position.x = hit.position.x;
			position.z = hit.position.z;
			velocity.x = 0;
			velocity.z = 0;			
		}
		else
		{
			//�ړ�
			position.x += mx;
			position.z += mz;
		}

	}
}

void Meteore::on_hit()
{
	is_hit = true;
}
