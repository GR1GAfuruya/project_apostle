#include "player.h"
#include "Operators.h"

///////////////////////////////////////////////////////
//
/*--------------------��ԑJ��------------------------*/
//
///////////////////////////////////////////////////////
void Player::transition_idle_state()
{
	p_update = &Player::update_idle_state;
	model->play_animation(PlayerAnimation::PLAYER_IDLE, true);
	state = State::IDLE;
}

void Player::transition_move_state()
{
	p_update = &Player::update_move_state;
	model->play_animation(PlayerAnimation::PLAYER_RUN, true);
	state = State::MOVE;
}

void Player::transition_avoidance_state()
{
	p_update = &Player::update_avoidance_state;
	model->play_animation(PlayerAnimation::PLAYER_ROLL, false, 0.1f);
	state = State::ROLL;
}

void Player::transition_jump_state()
{
	p_update = &Player::update_jump_state;
	model->play_animation(PlayerAnimation::PLAYER_JUMP, false, 0.1f);
	state = State::JUMP;
}

void Player::transition_damage_front_state()
{
	p_update = &Player::update_damage_front_state;
	model->play_animation(PlayerAnimation::PLAYER_DAMAGE_FRONT, false);
	state = State::FRONT_DAMAGE;
}

void Player::transition_r_attack_spring_slash_state()
{
	p_update = &Player::update_r_attack_spring_slash_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_SPRING_SLASH, false);
	add_impulse(Math::get_posture_forward(orientation) * 50.0f);
	state = State::SKILL;
}

void Player::transition_attack_pull_slash_state()
{
	p_update = &Player::update_attack_pull_slash_state;
	model->play_animation(PlayerAnimation::PLAYER_PULL_SLASH, false);
	state = State::SKILL;

}

void Player::transition_attack_ground_state()
{
	p_update = &Player::update_attack_ground_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_GROUND, false);
	//���[�g���[�V�������g�p���邩
	is_root_motion = false;
	state = State::SKILL;

}

void Player::transition_magic_buff_state()
{
	p_update = &Player::update_magic_buff_state;
	model->play_animation(PlayerAnimation::PLAYER_MAGIC_BUFF, false);
	//���[�g���[�V�������g�p���邩
	is_root_motion = false;
	state = State::SKILL;

}

void Player::transition_attack_bullet_state()
{
	p_update = &Player::update_attack_bullet_state;
	model->play_animation(PlayerAnimation::PLAYER_MAGIC_BULLET, false, 0.1f);
	//���[�g���[�V�������g�p���邩
	is_root_motion = false;
	state = State::SKILL;

}

void Player::transition_attack_slash_up_state()
{
	p_update = &Player::update_attack_slash_up_state;
	model->play_animation(PlayerAnimation::PLAYER_MAGIC_SLASH_UP, false);
	//���[�g���[�V�������g�p���邩
	is_root_motion = true;
	//���[�g���[�V�����p�_�~�[�|�W�V����
	root_motion_pos = position;
	state = State::SKILL;
}

void Player::transition_r_attack_forward_state()
{
	p_update = &Player::update_r_attack_forward_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_FORWARD_SLASH, false);
	//�O�i
	add_impulse(Math::get_posture_forward(orientation) * 50.0f);
	state = State::SKILL;
}

void Player::transition_attack_air_state()
{
	p_update = &Player::update_attack_air_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_AIR, false);
	state = State::SKILL;
}

void Player::transition_r_attack_combo1_state()
{
	p_update = &Player::update_r_attack_combo1_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_COMBO1, false, 0.1f);
	//�U���p�����[�^�[�ݒ�
	state = State::NORMAL_ATTACK;
	attack_sword_param = param.combo_1;
}

void Player::transition_r_attack_combo2_state()
{
	p_update = &Player::update_r_attack_combo2_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_COMBO2, false, 0.1f);
	//�U���p�����[�^�[�ݒ�
	state = State::NORMAL_ATTACK;
	//���[�g���[�V�������g�p���邩
	is_root_motion = true;
	//���[�g���[�V�����p�_�~�[�|�W�V����
	root_motion_pos = position;
	attack_sword_param = param.combo_2;
}

void Player::transition_r_attack_combo3_state()
{
	p_update = &Player::update_r_attack_combo3_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_COMBO3, false, 0.1f);
	state = State::NORMAL_ATTACK;
	//�U���p�����[�^�[�ݒ�
	attack_sword_param = param.combo_3;

}

void Player::transition_r_attack_dodge_back_state()
{
	p_update = &Player::update_r_attack_dodge_back_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_DODGE_BACK, false, 0.1f);
	//���Ɍ��
	add_impulse(Math::get_posture_forward(orientation) * -50.0f);

	state = State::SKILL;
}


///////////////////////////////////////////////////////
//
/*--------------------��ԍX�V------------------------*/
//
///////////////////////////////////////////////////////

//�ҋ@
void Player::update_idle_state(float elapsed_time, Camera* camera)
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
		transition_r_attack_combo1_state();
	}

	//�X�L������
	input_chant_support_skill(camera);
	input_chant_attack_skill(camera);

	//���͏����X�V
	update_velocity(elapsed_time, position);
}

//�ړ�
void Player::update_move_state(float elapsed_time, Camera* camera)
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
		transition_r_attack_combo1_state();
	}

	//�X�L������
	input_chant_support_skill(camera);
	input_chant_attack_skill(camera);

	//���͏����X�V
	update_velocity(elapsed_time, position);
}


//���
void Player::update_avoidance_state(float elapsed_time, Camera* camera)
{
	//���X�ɑ��x�𗎂Ƃ��Ă���
	velocity.x /= 2.0f;
	velocity.z /= 2.0f;
	//���͏����X�V
	update_velocity(elapsed_time, position);

	if (model->anime_param.frame_index > 33 / 2)
	{
		//�n�ʂɑ��������t���[������͂���ɑ��x���Ƃ�
		velocity.x /= 2.0f;
		velocity.z /= 2.0f;
	}
	else
	{
		//�����Ă�������ɑ��x�𑫂�
		velocity.x += (Math::get_posture_forward(orientation) * (param.avoidance_speed)).x;
		velocity.z += (Math::get_posture_forward(orientation) * (param.avoidance_speed)).z;

	}

	//�J�ڏ���
	if (model->anime_param.frame_index > 35 / 2)
	{
		//�W�����v�X�e�[�g�ֈڍs
		transition_jump_state();
		// MOVE�X�e�[�g�ֈڍs
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

//�W�����v
void Player::update_jump_state(float elapsed_time, Camera* camera)
{
	input_move(elapsed_time, camera);

	if (is_ground)
	{
		transition_idle_state();
	}

	//�U������
	if (game_pad->get_button_down() & game_pad->BTN_X)
	{
		transition_r_attack_combo1_state();
	}

	//������
	input_avoidance();

	//�X�L������
	input_chant_support_skill(camera);
	input_chant_attack_skill(camera);

	//���͏����X�V
	update_velocity(elapsed_time, position);
}

//���ʂ���̍U����H�炤
void Player::update_damage_front_state(float elapsed_time, Camera* camera)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
	}
	//���͏����X�V
	update_velocity(elapsed_time, position);
}

//��]�a��
void Player::update_r_attack_spring_slash_state(float elapsed_time, Camera* camera)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
	}
	//���͏����X�V
	update_velocity(elapsed_time, position);

}

//
void Player::update_attack_pull_slash_state(float elapsed_time, Camera* camera)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
	}

}

void Player::update_attack_ground_state(float elapsed_time, Camera* camera)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
	}
	//���͏����X�V
	update_velocity(elapsed_time, position);

}

//�o�t�l���A�j���[�V����
void Player::update_magic_buff_state(float elapsed_time, Camera* camera)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
	}
	//���͏����X�V
	update_velocity(elapsed_time, position);
}

//���@�e�ˏo�s��
void Player::update_attack_bullet_state(float elapsed_time, Camera* camera)
{
	//�G�̕���������
	DirectX::XMFLOAT3 target_vec = camera->get_lock_on_target() - position;
	target_vec.y = 0;
	Turn(elapsed_time, target_vec, chara_param.turn_speed * 2, orientation);
	if (model->is_end_animation())
	{
		transition_idle_state();
	}
	//���͏����X�V
	update_velocity(elapsed_time, position);
}

//
void Player::update_attack_slash_up_state(float elapsed_time, Camera* camera)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
	}
	//���͏����X�V
	update_velocity(elapsed_time, position);
}

void Player::update_r_attack_forward_state(float elapsed_time, Camera* camera)
{
	//*************************************//
	//	���[�g���[�V�����Ɋւ���X�V	   //
	//*************************************//
	root_motion_manual(Math::get_posture_forward(orientation), 1.5f * elapsed_time);
	//���͏����X�V
	update_velocity(elapsed_time, position);
}




void Player::update_attack_air_state(float elapsed_time, Camera* camera)
{
	if (model->anime_param.frame_index > 43 / 2)
	{
		//	attack_combo4_effect(graphics, elapsed_time);
	}
	if (model->is_end_animation())
	{
		transition_idle_state();
	}

	//���͏����X�V
	update_velocity(elapsed_time, position);
}
void Player::update_r_attack_combo1_state(float elapsed_time, Camera* camera)
{
	//*************************************//
	//			�U������Ɋւ���X�V	   //
	//*************************************//
	DirectX::XMFLOAT3 sword_pos;
	DirectX::XMFLOAT4X4 bone_ori = {};
	model->fech_by_bone(transform, root, sword_pos, &bone_ori);
	DirectX::XMFLOAT3 slash_dir = sword->get_dir_sword_top();
	DirectX::XMFLOAT3 up = { bone_ori._11,bone_ori._12,bone_ori._13 };
	if (model->anime_param.frame_index == 20 / 2)
	{
		slash_efects[0]->play(sword->get_equipped_position());
		slash_efects[0]->rotate_base_axis(MeshEffect::AXIS::FORWARD, slash_dir);
		slash_efects[0]->rotate_base_axis(MeshEffect::AXIS::UP, up);
		slash_efects[0]->rot_speed.y = -param.sword_swing_speed;
		//�U������ON
		attack_sword_param.is_attack = true;
		root_motion_manual(Math::get_posture_forward(orientation), param.floating_value);//���[�g���[�V��������Ȃ��蓮����

	}
	//*************************************//
	//			�J�ڂɊւ���X�V	   //
	//*************************************//

	if (model->anime_param.frame_index > 42 / 2)
	{
		if (model->is_end_animation())
		{
			transition_idle_state();
		}

		if (game_pad->get_button() & game_pad->BTN_X)
		{
			transition_r_attack_combo2_state();
		}


		//�U������OFF
		attack_sword_param.is_attack = false;
	}
	else
	{
		//�󒆂ɂ���ꍇ�������V
		if (floating())
		{
		}

	}
	input_move(elapsed_time, camera, param.floating_value, param.floating_value);
	//*************************************//
	//	���[�g���[�V�����Ɋւ���X�V	   //
	//*************************************//
	//������
	input_avoidance();
	//���͏����X�V
	update_velocity(elapsed_time, position);
}

void Player::update_r_attack_combo2_state(float elapsed_time, Camera* camera)
{
	DirectX::XMFLOAT3 sword_pos;
	DirectX::XMFLOAT4X4 bone_ori = {};
	model->fech_by_bone(transform, root, sword_pos, &bone_ori);
	DirectX::XMFLOAT3 up = Math::get_posture_up(orientation);
	DirectX::XMFLOAT3 slash_dir = sword->get_dir_sword_top();
	if (model->anime_param.frame_index == 12 / 2)
	{
		slash_efects[1]->play(sword->get_equipped_position());
		slash_efects[1]->rotate_base_axis(MeshEffect::AXIS::FORWARD, slash_dir);
		slash_efects[1]->rotate_base_axis(MeshEffect::AXIS::UP, up);
		slash_efects[1]->rot_speed.y = param.sword_swing_speed;

		//�U������ON
		attack_sword_param.is_attack = true;

	}

	if (model->anime_param.frame_index > 30 / 2)
	{
		if (model->is_end_animation())
		{
			transition_idle_state();

		}

		if (game_pad->get_button() & game_pad->BTN_X)
		{
			transition_r_attack_combo3_state();
		}
		//�U������OFF
		attack_sword_param.is_attack = false;


	}
	//*************************************//
	//	���[�g���[�V�����Ɋւ���X�V	   //
	//*************************************//
	root_motion_manual(Math::get_posture_forward(orientation), -10.0f * elapsed_time);
	//������
	input_avoidance();
	//�󒆂ɂ���ꍇ�������V
	floating();

	//���͏����X�V
	update_velocity(elapsed_time, position);
}

void Player::update_r_attack_combo3_state(float elapsed_time, Camera* camera)
{
	DirectX::XMFLOAT3 sword_pos;
	DirectX::XMFLOAT4X4 bone_ori = {};
	model->fech_by_bone(transform, root, sword_pos, &bone_ori);
	DirectX::XMFLOAT3 slash_dir = sword->get_dir_sword_top();
	DirectX::XMFLOAT3 up = { bone_ori._11,bone_ori._12,bone_ori._13 };
	//��U��ڂ̎a��
	if (model->anime_param.frame_index == 4 / 2)
	{
		slash_efects[0]->play(sword->get_equipped_position());
		slash_efects[0]->rotate_base_axis(MeshEffect::AXIS::FORWARD, slash_dir);
		slash_efects[0]->rotate_base_axis(MeshEffect::AXIS::UP, up);
		slash_efects[0]->rot_speed.y = -param.sword_swing_speed;
		//�U������ON
		attack_sword_param.is_attack = true;
		const float impulse_power = combo3_impulse1 * elapsed_time;
		add_impulse(Math::get_posture_forward(orientation) * impulse_power);
	}
	//��U��ڂ̎a��
	if (model->anime_param.frame_index == 30 / 2)
	{
		slash_efects[1]->play(sword->get_equipped_position());
		slash_efects[1]->rotate_base_axis(MeshEffect::AXIS::FORWARD, slash_dir);
		slash_efects[1]->rotate_base_axis(MeshEffect::AXIS::UP, up);
		slash_efects[1]->rot_speed.y = param.sword_swing_speed;
		const float impulse_power = combo3_impulse2 * elapsed_time;
		add_impulse(Math::get_posture_forward(orientation) * impulse_power);
	}
	//�O�U��ڂ̎a��
	if (model->anime_param.frame_index == 50 / 2)
	{
		slash_efects[2]->play(sword->get_equipped_position());
		slash_efects[2]->rotate_base_axis(MeshEffect::AXIS::FORWARD, slash_dir);
		slash_efects[2]->rotate_base_axis(MeshEffect::AXIS::UP, up);
		slash_efects[2]->rot_speed.y = -param.sword_swing_speed;
		velocity.x /= 5.0f;
		velocity.z /= 5.0f;
	}

	//3�A���ڂ܂ł͕���
	if (model->anime_param.frame_index < 50 / 2)
	{
		//�󒆂ɂ���ꍇ�������V
		floating();

	}


	if (model->anime_param.frame_index > 66 / 2)
	{
		if (model->anime_param.frame_index > 95 / 2 || model->is_end_animation())
		{
			transition_idle_state();
		}

		//�U������OFF
		attack_sword_param.is_attack = false;
	}


	//������
	input_avoidance();

	//���͏����X�V
	update_velocity(elapsed_time, position);
}

void Player::update_r_attack_dodge_back_state(float elapsed_time, Camera* camera)
{
	//*************************************//
	//	���[�g���[�V�����Ɋւ���X�V	   //
	//*************************************//
	float speed = model->root_defference_length_next_frame(root);
	velocity = Math::get_posture_forward(orientation) * (speed * add_root_speed);
}



