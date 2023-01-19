#include"charactor.h"

#include "stage_manager.h"
#include "user.h"


void Charactor::add_impulse(const DirectX::XMFLOAT3& impulse)
{
	//���͂ɗ͂�������
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;

}

bool Charactor::apply_damage(int damage, float invinsibleTime, WINCE_TYPE type)
{
	//�_���[�W��0�̏ꍇ�͌��N��Ԃ�ύX����K�v���Ȃ�
	if (damage == 0)return false;

	//���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
	if (health <= 0)return false;


	if (invincible_timer > 0.0f)return false;

	//���G���Ԑݒ�
	invincible_timer = invinsibleTime;
	//�_���[�W����
	health -= damage;

	//���S�ʒm
	if (health <= 0)
	{
		on_dead();
	}
	else//�_���[�W�ʒm
	{
		on_damaged(type);
	}

	//���N��Ԃ��ύX�����ꍇ��true��Ԃ�
	return true;
}





void Charactor::Move(float vx, float vz, float speed)
{
	//�ړ������x�N�g����ݒ�
	move_vec_x = vx;
	move_vec_z = vz;

	//�ő呬�x�ݒ�
	chara_param.max_move_speed = speed;
}

void Charactor::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;
	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f) return;

	vx /= length;
	vz /= length;


	const float forwardX = sinf(angle.y);
	const float forwardZ = cosf(angle.y);

	//�x�N�g���`�Ƃa�̊O��
	//A.x*B.z - A.z*B.x
	float cross = forwardX * vz - forwardZ * vx;

	//�x�N�g���`�Ƃa�̓���
	//A.x*B.x - A.z*B.z
	float dot = forwardX * vx + forwardZ * vz;
	float rot = 1 - dot;

	if (rot > speed) rot = speed;


	if (cross < 0.0f)//�E�ɐ���
	{
		angle.y += rot;
	}

	else //���ɐ���
	{
		angle.y -= rot;
	}

	angle.x = fmod(angle.x, DirectX::XMConvertToRadians(360.0f));
	angle.y = fmod(angle.y, DirectX::XMConvertToRadians(360.0f));
	angle.z = fmod(angle.z, DirectX::XMConvertToRadians(360.0f));
}

void Charactor::Turn(float elapsed_time, DirectX::XMFLOAT3 move_vec, float speed, DirectX::XMFLOAT4& orien)
{
	// XMVECTOR�N���X�֕ϊ�
	DirectX::XMVECTOR orientationVec = DirectX::XMLoadFloat4(&orien);
	DirectX::XMVECTOR MoveVec = DirectX::XMLoadFloat3(&move_vec);
	if (DirectX::XMVector3Equal(MoveVec, DirectX::XMVectorZero())) return; //����move_vec���[���x�N�g���Ȃ烊�^�[��
	DirectX::XMVECTOR forward, up;

	up = Math::get_posture_up_vec(orien);
	forward = Math::get_posture_forward_vec(orien);

	up = DirectX::XMVector3Normalize(up);
	forward = DirectX::XMVector3Normalize(forward);

	//���K��
	MoveVec = DirectX::XMVector3Normalize(MoveVec);

	DirectX::XMVECTOR axis;	//��]��
	
	axis = up;
	DirectX::XMVECTOR Ang = DirectX::XMVector3Dot(forward, MoveVec);
	DirectX::XMStoreFloat(&turn_angle, Ang);
	turn_angle = acosf(turn_angle);

	DirectX::XMFLOAT3 forw{};//forward�̒l��float3��
	DirectX::XMFLOAT3 m_vec{};//d�̒l��float3��

	DirectX::XMStoreFloat3(&forw, forward);
	DirectX::XMStoreFloat3(&m_vec, MoveVec);
	//��]�p�iangle�j�������ȏꍇ�́A��]���s��Ȃ�
	if (fabs(turn_angle) > 1e-8f)
	{
		//��]���iaxis�j�Ɖ�]�p�iangle�j�����]�N�I�[�^�j�I���iq�j�����߂�
		float cross{ forw.x * m_vec.z - forw.z * m_vec.x };

		//�N�I�[�^�j�I���͉�]�̎d��(�ǂ̌�����)
		const float rate = speed;
		if (cross < 0.0f)
		{
			//��]���Ɖ�]�p�����]�N�I�[�^�j�I�������߂�
			DirectX::XMVECTOR q;
			q = DirectX::XMQuaternionRotationAxis(axis, turn_angle);//���̕����ɓ����N�I�[�^�j�I��

			DirectX::XMVECTOR End = DirectX::XMQuaternionMultiply(orientationVec, q);
			orientationVec = DirectX::XMQuaternionSlerp(orientationVec, End, rate * elapsed_time);
		}
		else
		{
			DirectX::XMVECTOR q;
			q = DirectX::XMQuaternionRotationAxis(axis, -turn_angle);//���̕����ɓ����N�I�[�^�j�I��
			DirectX::XMVECTOR End = DirectX::XMQuaternionMultiply(orientationVec, q);
			orientationVec = DirectX::XMQuaternionSlerp(orientationVec, End, rate * elapsed_time);
		}

	}

	// orientationVec����orientation���X�V
	DirectX::XMStoreFloat4(&orientation, orientationVec);
}

void Charactor::Jump(float speed)
{
	//������̗͂�ݒ�
	velocity.y = speed;
}
void Charactor::update_velocity(float elapsed_time, DirectX::XMFLOAT3& position)
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

void Charactor::update_vertical_velocity(float elapsed_frame)
{
	velocity.y += gravity * elapsed_frame;
}

void Charactor::update_vertical_move(float elapsed_time, DirectX::XMFLOAT3& position)
{
	// �L�����N�^�[�̉������̈ړ���
	float my = velocity.y * elapsed_time;


	// �L�����N�^�[��Y�������ƂȂ�@���x�N�g��
	slope_normal = { 0, 1, 0 };
	// ������
	if (my < 0.0f)
	{
		//���C�̊J�n�ʒu�͑�����菭����
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		//���C�̏I�_�ʒu�͈ړ���̈ʒu
		DirectX::XMFLOAT3 end = { position.x, position.y + my  , position.z };

		//���C�L���X�g�ɂ��n�ʔ���
		HitResult hit;
		if (StageManager::Instance().ray_cast(start, end, hit))
		{
			//�n�ʂɐݒu���Ă���
			position = hit.position;

			//angle.y += hit.rotation.y;
			// �@���x�N�g���擾
			//normal = hit.normal;
			//���n����
			if (!is_ground)
			{
				on_landing();
			}
			is_ground = true;
			velocity.y = 0.0f;
		}
		else
		{
			//�󒆂ɕ����Ă���
			position.y += my;
			is_ground = false;
		}

	}
	// �㏸��
	else if (my > 0.0f)
	{
		position.y += my;
		is_ground = false;
	}

	// �n�ʂ̌����ɉ����悤��XZ����]
	{

		DirectX::XMVECTOR OrientationVec = DirectX::XMLoadFloat4(&orientation);

		//��x�N�g��
		DirectX::XMVECTOR up = Math::get_posture_up_vec(orientation);
		//�@���̃x�N�g��
		DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&slope_normal);
		Normal = DirectX::XMVector3Normalize(Normal);
		//���x�N�g���Z�o
		DirectX::XMVECTOR axis;	//��]��
		float angle;			//��]�p
		axis = DirectX::XMVector3Cross(up, Normal);
		DirectX::XMVECTOR Ang = DirectX::XMVector3Dot(up, Normal);
		DirectX::XMStoreFloat(&angle, Ang);
		angle = acosf(angle);
		//�����[���x�N�g���Ȃ烊�^�[��
		if (DirectX::XMVector3Equal(axis, DirectX::XMVectorZero())) return;
		//��]�p�iangle�j�������ȏꍇ�́A��]���s��Ȃ�
		if (fabs(angle) > 1e-8f)
		{
			DirectX::XMVECTOR q;
			q = DirectX::XMQuaternionRotationAxis(axis, angle);
			DirectX::XMVECTOR End = DirectX::XMQuaternionMultiply(OrientationVec, q);
			float rate = 10.0f;
			OrientationVec = DirectX::XMQuaternionSlerp(OrientationVec, End, rate * elapsed_time);
		}
		// orientationVec����orientation���X�V
		DirectX::XMStoreFloat4(&orientation, OrientationVec);

	}


}

void Charactor::update_hrizontal_velocity(float elapsed_frame)
{
	//XZ���ʂ̑��͂���������
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length > 0.0f)
	{
		float friction = chara_param.friction * elapsed_frame;

		//if (IsGround()) friction += airControl;
		//���C�ɂ�鉡�����̌�������
		if (length > friction)
		{
			(velocity.x < 0.0f) ? velocity.x += friction : velocity.x -= friction;
			(velocity.z < 0.0f) ? velocity.z += friction : velocity.z -= friction;
		}
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}

	}
	//XZ���ʂ̑��͂���������
	if (length <= chara_param.max_move_speed)
	{
		//�ړ��x�N�g�����[���x�N�g���łȂ��Ȃ��������
		float moveVecLength = sqrtf(move_vec_x * move_vec_x + move_vec_z * move_vec_z);


		if (moveVecLength > 0.0f)
		{
			//������
			float acceleration = chara_param.acceleration * elapsed_frame;
			//�󒆂ɂ���Ƃ��͉����͂����炷
			if (get_is_ground()) acceleration += chara_param.air_control;
			//�ړ��x�N�g���ɂ���������
			velocity.x += move_vec_x * acceleration;
			velocity.z += move_vec_z * acceleration;

			//�ő呬�x����
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > chara_param.max_move_speed)
			{
				float vx = velocity.x / length;
				float vz = velocity.z / length;
				velocity.x = vx * chara_param.max_move_speed;
				velocity.z = vz * chara_param.max_move_speed;
			}
		}
	}
	else
	{
		move_vec_x = 0.0f;
		move_vec_z = 0.0f;
	}
}


void Charactor::update_horizontal_move(float elapsed_time,DirectX::XMFLOAT3& position)
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
		DirectX::XMFLOAT3 start = { position.x - mx / 50.0f, position.y + stepOffset * 2, position.z - mz / 50.0f };
		DirectX::XMFLOAT3 end = { position.x + mx* vs_wall_ray_power, start.y, position.z + mz * vs_wall_ray_power };
		HitResult hit;
		if (StageManager::Instance().ray_cast(start, end, hit))//�����ǂ������
		{
			//�ǂ܂ł̃x�N�g��
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

			//���˃x�N�g����@���Ɏˉe
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal); //���̎��_�ł͐������t�Ȃ��Ƃɒ���

			//�␳�ʒu�̌v�Z
			DirectX::XMVECTOR Correct = DirectX::XMVectorMultiplyAdd(Normal, Dot, End);//End�Ɏˉe�x�N�g���𑫂�



			DirectX::XMFLOAT3 correct;
			DirectX::XMStoreFloat3(&correct, Correct);

			HitResult hit2;
			if (!StageManager::Instance().ray_cast(start, correct, hit2))//�����ǂ������
			{
				position.x = correct.x;
				position.z = correct.z;
				velocity.x = 0;
				velocity.z = 0;
			}
			else
			{
				position.x = hit2.position.x;
				position.z = hit2.position.z;
				velocity.x = 0;
				velocity.z = 0;
			}
		}
		else
		{
			//�ړ�
			position.x += mx;
			position.z += mz;
		}
		
	}
}



void Charactor::update_invicible_timer(float elapsedTime)
{
	if (invincible_timer > 0.0f)
	{
		invincible_timer -= elapsedTime;
	}
	else
	{
		invincible_timer = 0.0f;
	}
}
