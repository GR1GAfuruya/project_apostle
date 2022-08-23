#include"player_move.h"

#include "stage_manager.h"
#include "user.h"

//�s��X�V����
void PlayerMove::update_transform()
{
	//�X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//��]�s����쐬
	//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	//DirectX::XMMATRIX R = Y * X * Z;
	DirectX::XMMATRIX R = Y * X * Z;
	//�ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//3�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;
	//�v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, W);
}

//�N�I�[�^�j�I���p��UpdateTransform
void PlayerMove::update_transform(const DirectX::XMFLOAT4& orien, const DirectX::XMFLOAT3& pos)
{
	//�X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//��]�s����쐬
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&orien));
	//�ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//3�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;
	//�v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, W);

}


//�E�x�N�g���擾
DirectX::XMVECTOR PlayerMove::get_posture_right_vec(DirectX::XMFLOAT4 orientation)
{
	DirectX::XMVECTOR orientationVec = DirectX::XMLoadFloat4(&orientation);
	DirectX::XMVECTOR right;
	DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientationVec);
	DirectX::XMFLOAT4X4 m4x4 = {};
	DirectX::XMStoreFloat4x4(&m4x4, m);

	right = { m4x4._11, m4x4._12, m4x4._13 };
	right = DirectX::XMVector3Normalize(right);
	return right;
}

//��x�N�g���擾
DirectX::XMVECTOR PlayerMove::get_posture_up_vec(DirectX::XMFLOAT4 orientation)
{
	DirectX::XMVECTOR orientationVec = DirectX::XMLoadFloat4(&orientation);
	DirectX::XMVECTOR up;
	DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientationVec);
	DirectX::XMFLOAT4X4 m4x4 = {};
	DirectX::XMStoreFloat4x4(&m4x4, m);

	up = { m4x4._21, m4x4._22, m4x4._23 };
	up = DirectX::XMVector3Normalize(up);
	return up;
}

//�O�x�N�g���擾
DirectX::XMVECTOR PlayerMove::get_posture_forward_vec(DirectX::XMFLOAT4 orientation)
{
	DirectX::XMVECTOR orientationVec = DirectX::XMLoadFloat4(&orientation);
	DirectX::XMVECTOR forward;
	DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientationVec);
	DirectX::XMFLOAT4X4 m4x4 = {};
	DirectX::XMStoreFloat4x4(&m4x4, m);
	forward = { m4x4._31, m4x4._32, m4x4._33 };
	forward = DirectX::XMVector3Normalize(forward);
	return forward;
}

//SceneGrath �e�q�s��
#if 0
void Character::update_transform(const DirectX::XMFLOAT4& orien, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4X4& tran)
{
	//�X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//��]�s����쐬
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&orien));
	//�ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x - pos.x, position.y - pos.y, position.z - pos.z);
	//3�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMMATRIX tra = DirectX::XMLoadFloat4x4(&tran);
	W = tra * W;
	//�v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, W);
}
#endif // 0


void PlayerMove::add_impulse(const DirectX::XMFLOAT3& impulse)
{
	//���͂ɗ͂�������
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;

}

bool PlayerMove::apply_damage(int damage, float invinsibleTime)
{
	//�_���[�W��0�̏ꍇ�͌��N��Ԃ�ύX����K�v���Ȃ�
	if (damage == 0)return false;

	//���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
	if (health <= 0)return false;


	if (invinsible_timer > 0.0f)return false;

	//���G���Ԑݒ�
	invinsible_timer = invinsibleTime;
	//�_���[�W����
	health -= damage;

	//���S�ʒm
	if (health <= 0)
	{
		//OnDead();
	}
	else//�_���[�W�ʒm
	{
		on_damaged();
	}

	//���N��Ԃ��ύX�����ꍇ��true��Ԃ�
	return true;
}





void PlayerMove::Move(float vx, float vz, float speed)
{
	//�ړ������x�N�g����ݒ�
	move_vec_x = vx;
	move_vec_z = vz;

	//�ő呬�x�ݒ�
	max_move_speed = speed;
}

void PlayerMove::Turn(float elapsedTime, float vx, float vz, float speed)
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

void PlayerMove::Turn(float elapsed_time, DirectX::XMFLOAT3 move_vec, float speed, DirectX::XMFLOAT4& orien)
{
	// XMVECTOR�N���X�֕ϊ�
	DirectX::XMVECTOR orientationVec = DirectX::XMLoadFloat4(&orien);
	DirectX::XMVECTOR MoveVec = DirectX::XMLoadFloat3(&move_vec);
	if (DirectX::XMVector3Equal(MoveVec, DirectX::XMVectorZero())) return; //����move_vec���[���x�N�g���Ȃ烊�^�[��
	DirectX::XMVECTOR forward, up;

	up = get_posture_up_vec(orien);
	forward = get_posture_forward_vec(orien);

	up = DirectX::XMVector3Normalize(up);
	forward = DirectX::XMVector3Normalize(forward);

	//���K��
	MoveVec = DirectX::XMVector3Normalize(MoveVec);

	DirectX::XMVECTOR axis;	//��]��
	float angle;			//��]�p
	axis = up;
	DirectX::XMVECTOR Ang = DirectX::XMVector3Dot(forward, MoveVec);
	DirectX::XMStoreFloat(&angle, Ang);
	angle = acosf(angle);

	DirectX::XMFLOAT3 forw{};//forward�̒l��float3��
	DirectX::XMFLOAT3 m_vec{};//d�̒l��float3��

	DirectX::XMStoreFloat3(&forw, forward);
	DirectX::XMStoreFloat3(&m_vec, MoveVec);
	//��]�p�iangle�j�������ȏꍇ�́A��]���s��Ȃ�
	if (fabs(angle) > 1e-8f)
	{
		//��]���iaxis�j�Ɖ�]�p�iangle�j�����]�N�I�[�^�j�I���iq�j�����߂�
		float cross{ forw.x * m_vec.z - forw.z * m_vec.x };

		//�N�I�[�^�j�I���͉�]�̎d��(�ǂ̌�����)
		const float rate = 10.0f;
		if (cross < 0.0f)
		{
			//��]���Ɖ�]�p�����]�N�I�[�^�j�I�������߂�
			DirectX::XMVECTOR q;
			q = DirectX::XMQuaternionRotationAxis(axis, angle);//���̕����ɓ����N�I�[�^�j�I��

			DirectX::XMVECTOR End = DirectX::XMQuaternionMultiply(orientationVec, q);
			orientationVec = DirectX::XMQuaternionSlerp(orientationVec, End, rate * elapsed_time);
		}
		else
		{
			DirectX::XMVECTOR q;
			q = DirectX::XMQuaternionRotationAxis(axis, -angle);//���̕����ɓ����N�I�[�^�j�I��
			DirectX::XMVECTOR End = DirectX::XMQuaternionMultiply(orientationVec, q);
			orientationVec = DirectX::XMQuaternionSlerp(orientationVec, End, rate * elapsed_time);
		}

	}

	// orientationVec����orientation���X�V
	DirectX::XMStoreFloat4(&orientation, orientationVec);
}

void PlayerMove::Jump(float speed)
{
	//������̗͂�ݒ�
	velocity.y = speed;
}
void PlayerMove::update_velocity(float elapsedTime, DirectX::XMFLOAT3& position, Stage* stage)
{
	//�o�߃t���[��
	float elapsedFrame = 60.0f * elapsedTime;


	//�������͍X�V����
	update_vertical_velocity(elapsedFrame);

	//�����ړ��X�V����
	update_vertical_move(elapsedTime, position, stage);

	//�������͍X�V����
	update_hrizontal_velocity(elapsedFrame);

	//�����ړ��X�V����
	update_horizontal_move(elapsedTime, position, stage);

}

void PlayerMove::update_vertical_velocity(float elapsedFrame)
{
	velocity.y += gravity * elapsedFrame;
}

using namespace DirectX;
void PlayerMove::update_vertical_move(float elapsed_time, DirectX::XMFLOAT3& position, Stage* stage)
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
		if (StageManager::Instance().Raycast(start, end, hit))
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
		DirectX::XMVECTOR up = get_posture_up_vec(orientation);
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

void PlayerMove::update_hrizontal_velocity(float elapsed_frame)
{
	//XZ���ʂ̑��͂���������
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length > 0.0f)
	{
		float friction = this->friction * elapsed_frame;

		//if (IsGround()) friction += airControl;
		//���C�ɂ�鉡�����̌�������
		if (length > friction)
		{
			float vx = velocity.x / length;
			float vz = velocity.z / length;
			velocity.x -= vx * friction;
			velocity.z -= vz * friction;
		}
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}

	}
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
			if (get_is_ground()) acceleration += air_control;
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


void PlayerMove::update_horizontal_move(float elapsed_time,DirectX::XMFLOAT3& position, Stage* stage)
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
		DirectX::XMFLOAT3 start = { position.x - mx / 50.0f, position.y + stepOffset, position.z - mz / 50.0f };
		DirectX::XMFLOAT3 end = { position.x + mx* 5.0f, start.y, position.z + mz * 5.0f};
		HitResult hit;
		if (StageManager::Instance().Raycast(start, end, hit))//�����ǂ������
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
			if (!StageManager::Instance().Raycast(start, correct, hit2))//�����ǂ������
			{
				position.x = correct.x;
				//velocity.x = 0;
				position.z = correct.z;
				//velocity.z = 0;
			}
			else
			{
				position.x = hit2.position.x;
				position.z = hit2.position.z;
			}
		}
		else
		{
			//�ړ�
			position.x += mx;
			position.z += mz;
		}
		
	}
	////�ړ�����
	//position.x += velocity.x * elapsed_time;
	//position.z += velocity.z * elapsed_time;
}



void PlayerMove::update_invicible_timer(float elapsedTime)
{
	if (invinsible_timer > 0.0f)
	{
		invinsible_timer -= elapsedTime;
	}
	else
	{
		invinsible_timer = 0.0f;
	}
}