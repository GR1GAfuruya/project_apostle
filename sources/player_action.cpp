#include "player.h"
#include "Operators.h"


void Player::transition_idle_state()
{
	p_update = &Player::update_idle_state;
	model->play_animation(PlayerAnimation::PLAYER_IDLE, true);
}

void Player::transition_attack_combo4_state()
{
	p_update = &Player::update_attack_combo4_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_AIR, false);

}

void Player::transition_attack_combo1_state()
{
	p_update = &Player::update_attack_combo1_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_COMBO1, false, 0.1);
}

void Player::transition_attack_combo2_state()
{
	p_update = &Player::update_attack_combo2_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_COMBO2, false, 0.1);
}

void Player::transition_attack_combo3_state()
{
	p_update = &Player::update_attack_combo3_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_COMBO3, false, 0.1);
}

void Player::transition_move_state()
{
	p_update = &Player::update_move_state;
	model->play_animation(PlayerAnimation::PLAYER_WALK, true);
}

void Player::transition_jump_state()
{
	p_update = &Player::update_jump_state;
	model->play_animation(PlayerAnimation::PLAYER_JUMP, false, 0.1);
}

void Player::transition_avoidance_state()
{
	p_update = &Player::update_avoidance_state;
	model->play_animation(PlayerAnimation::PLAYER_ROLL, false, 0.1f);
}

void Player::update_idle_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	if (input_move(elapsed_time, camera))
	{
		transition_move_state();
	}
	//�W�����v����
	input_jump();
	//������
	input_avoidance();

	//�U������
	if (game_pad->get_button_down() & game_pad->BTN_X)
	{
		transition_attack_combo1_state();
	}

	//���͏����X�V
	update_velocity(elapsed_time, position, stage);
}

void Player::update_attack_combo1_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	DirectX::XMFLOAT3 sword_pos;
	DirectX::XMFLOAT4X4 bone_ori = {};
	model->fech_by_bone(transform, sword_hand, sword_pos, &bone_ori);
	DirectX::XMFLOAT3 slash_dir = Math::get_posture_forward(orientation);
	DirectX::XMFLOAT3 up = { bone_ori._11,bone_ori._12,bone_ori._13 };
	if (model->anime_param.frame_index == 20 / 2)
	{
		slash_efect->play(sword_pos + (up * 3.2f), slash_dir, 45, false);
	}
	if (model->anime_param.frame_index > 40 / 2)
	{
		if (model->is_end_animation())
		{
			transition_idle_state();
		}

		if (game_pad->get_button() & game_pad->BTN_X)
		{
			transition_attack_combo2_state();
		}
	}

	//���͏����X�V
	update_velocity(elapsed_time, position, stage);
}

void Player::update_attack_combo2_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	DirectX::XMFLOAT3 sword_pos;
	DirectX::XMFLOAT4X4 bone_ori = {};
	model->fech_by_bone(transform, sword_hand, sword_pos, &bone_ori);
	DirectX::XMFLOAT3 slash_dir = Math::get_posture_forward(orientation);
	DirectX::XMFLOAT3 up = { bone_ori._11,bone_ori._12,bone_ori._13 };
	if (model->anime_param.frame_index == 17 / 2)
	{
		slash_efect->play(sword_pos + (up * 1.2f), slash_dir,10, true);
	}

	if (model->anime_param.frame_index > 45 / 2)
	{
		if (model->is_end_animation())
		{
			transition_idle_state();

		}

		if (game_pad->get_button() & game_pad->BTN_X)
		{
			transition_attack_combo3_state();
		}

	}
	//���͏����X�V
	update_velocity(elapsed_time, position, stage);
}

void Player::update_attack_combo3_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	DirectX::XMFLOAT3 sword_pos, up;
	model->fech_by_bone(transform, sword_hand, sword_pos);
	DirectX::XMFLOAT3 slash_dir_vec = Math::get_posture_forward(orientation);
	//��U��ڂ̎a��
	if (model->anime_param.frame_index == 20 / 2)
		slash_efect->play(sword_pos, slash_dir_vec, 25, false);
	//��U��ڂ̎a��
	if (model->anime_param.frame_index == 30 / 2)
		slash_efect->play(sword_pos, slash_dir_vec, 0, true);
	//�O�U��ڂ̎a��
	if (model->anime_param.frame_index == 55 / 2)
		slash_efect->play(sword_pos, slash_dir_vec, -25, false);

	if (model->anime_param.frame_index > 75 / 2)
	{
		if (model->anime_param.frame_index > 95 / 2 || model->is_end_animation())
		{
			transition_idle_state();
		}

		if (game_pad->get_button() & game_pad->BTN_X)
		{
			transition_attack_combo4_state();
		}

	}
	//���͏����X�V
	update_velocity(elapsed_time, position, stage);
}

void Player::update_attack_combo4_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	if (model->anime_param.frame_index > 43 / 2)
	{
		Attack(graphics, elapsed_time);
	}
	if (model->is_end_animation())
	{
		transition_idle_state();
	}

	//���͏����X�V
	update_velocity(elapsed_time, position, stage);
}

void Player::update_move_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	if (!input_move(elapsed_time, camera) && is_ground)
	{
		transition_idle_state();
	}

	input_jump();
	input_avoidance();

	//�U������
	if (game_pad->get_button() & game_pad->BTN_X)
	{
		transition_attack_combo1_state();
	}

	//���͏����X�V
	update_velocity(elapsed_time, position, stage);
}

void Player::update_jump_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	input_move(elapsed_time, camera);

	if (is_ground)
	{
		transition_idle_state();
	}

	//���͏����X�V
	update_velocity(elapsed_time, position, stage);
}

//���
void Player::update_avoidance_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	//���X�ɑ��x�𗎂Ƃ��Ă���
	velocity.x /= 2 ;
	velocity.z /= 2 ;
	//���͏����X�V
	update_velocity(elapsed_time, position, stage);
	if (model->anime_param.frame_index > 33 / 2)
	{
		//�n�ʂɑ��������t���[������͂���ɑ��x���Ƃ�
		velocity.x /= 2;
		velocity.z /= 2;
	}
	else
	{
		//�����Ă�������ɑ��x�𑫂�
		velocity.x += (Math::get_posture_forward(orientation) * (avoidance_speed)).x ;
		velocity.z += (Math::get_posture_forward(orientation) * (avoidance_speed)).z ;

	}

	//�J�ڏ���
	if (model->anime_param.frame_index > 44 / 2)
	{
		if (input_move(elapsed_time, camera))
		{
			transition_move_state();
			return;
		}
	}

	if (model->is_end_animation())
	{
		transition_idle_state();
	}

}
