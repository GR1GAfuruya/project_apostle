#include "player.h"
#include "Operators.h"

///////////////////////////////////////////////////////
//
/*--------------------状態遷移------------------------*/
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
	//ルートモーションを使用するか
	is_root_motion = false;
	state = State::SKILL;

}

void Player::transition_magic_buff_state()
{
	p_update = &Player::update_magic_buff_state;
	model->play_animation(PlayerAnimation::PLAYER_MAGIC_BUFF, false);
	//ルートモーションを使用するか
	is_root_motion = false;
	state = State::SKILL;

}

void Player::transition_attack_bullet_state()
{
	p_update = &Player::update_attack_bullet_state;
	model->play_animation(PlayerAnimation::PLAYER_MAGIC_BULLET, false, 0.1f);
	//ルートモーションを使用するか
	is_root_motion = false;
	state = State::SKILL;

}

void Player::transition_attack_slash_up_state()
{
	p_update = &Player::update_attack_slash_up_state;
	model->play_animation(PlayerAnimation::PLAYER_MAGIC_SLASH_UP, false);
	//ルートモーションを使用するか
	is_root_motion = true;
	//ルートモーション用ダミーポジション
	root_motion_pos = position;
	state = State::SKILL;
}

void Player::transition_r_attack_forward_state()
{
	p_update = &Player::update_r_attack_forward_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_FORWARD_SLASH, false);
	//前進
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
	//攻撃パラメーター設定
	state = State::NORMAL_ATTACK;
	attack_sword_param = param.combo_1;
}

void Player::transition_r_attack_combo2_state()
{
	p_update = &Player::update_r_attack_combo2_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_COMBO2, false, 0.1f);
	//攻撃パラメーター設定
	state = State::NORMAL_ATTACK;
	//ルートモーションを使用するか
	is_root_motion = true;
	//ルートモーション用ダミーポジション
	root_motion_pos = position;
	attack_sword_param = param.combo_2;
}

void Player::transition_r_attack_combo3_state()
{
	p_update = &Player::update_r_attack_combo3_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_COMBO3, false, 0.1f);
	state = State::NORMAL_ATTACK;
	//攻撃パラメーター設定
	attack_sword_param = param.combo_3;

}

void Player::transition_r_attack_dodge_back_state()
{
	p_update = &Player::update_r_attack_dodge_back_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_DODGE_BACK, false, 0.1f);
	//後ろに交代
	add_impulse(Math::get_posture_forward(orientation) * -50.0f);

	state = State::SKILL;
}


///////////////////////////////////////////////////////
//
/*--------------------状態更新------------------------*/
//
///////////////////////////////////////////////////////

//待機
void Player::update_idle_state(float elapsed_time, Camera* camera)
{
	if (input_move(elapsed_time, camera))
	{
		transition_move_state();
	}
	//ジャンプ入力
	input_jump();
	//回避入力
	input_avoidance();

	//攻撃入力
	if (game_pad->get_button_down() & game_pad->BTN_X)
	{
		transition_r_attack_combo1_state();
	}

	//スキル入力
	input_chant_support_skill(camera);
	input_chant_attack_skill(camera);

	//速力処理更新
	update_velocity(elapsed_time, position);
}

//移動
void Player::update_move_state(float elapsed_time, Camera* camera)
{
	if (!input_move(elapsed_time, camera) && is_ground)
	{
		transition_idle_state();
	}

	input_jump();
	input_avoidance();

	//攻撃入力
	if (game_pad->get_button() & game_pad->BTN_X)
	{
		transition_r_attack_combo1_state();
	}

	//スキル入力
	input_chant_support_skill(camera);
	input_chant_attack_skill(camera);

	//速力処理更新
	update_velocity(elapsed_time, position);
}


//回避
void Player::update_avoidance_state(float elapsed_time, Camera* camera)
{
	//徐々に速度を落としていく
	velocity.x /= 2.0f;
	velocity.z /= 2.0f;
	//速力処理更新
	update_velocity(elapsed_time, position);

	if (model->anime_param.frame_index > 33 / 2)
	{
		//地面に足がついたフレームからはさらに速度落とす
		velocity.x /= 2.0f;
		velocity.z /= 2.0f;
	}
	else
	{
		//向いている方向に速度を足す
		velocity.x += (Math::get_posture_forward(orientation) * (param.avoidance_speed)).x;
		velocity.z += (Math::get_posture_forward(orientation) * (param.avoidance_speed)).z;

	}

	//遷移処理
	if (model->anime_param.frame_index > 35 / 2)
	{
		//ジャンプステートへ移行
		transition_jump_state();
		// MOVEステートへ移行
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

//ジャンプ
void Player::update_jump_state(float elapsed_time, Camera* camera)
{
	input_move(elapsed_time, camera);

	if (is_ground)
	{
		transition_idle_state();
	}

	//攻撃入力
	if (game_pad->get_button_down() & game_pad->BTN_X)
	{
		transition_r_attack_combo1_state();
	}

	//回避入力
	input_avoidance();

	//スキル入力
	input_chant_support_skill(camera);
	input_chant_attack_skill(camera);

	//速力処理更新
	update_velocity(elapsed_time, position);
}

//正面からの攻撃を食らう
void Player::update_damage_front_state(float elapsed_time, Camera* camera)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
	}
	//速力処理更新
	update_velocity(elapsed_time, position);
}

//回転斬り
void Player::update_r_attack_spring_slash_state(float elapsed_time, Camera* camera)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
	}
	//速力処理更新
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
	//速力処理更新
	update_velocity(elapsed_time, position);

}

//バフ獲得アニメーション
void Player::update_magic_buff_state(float elapsed_time, Camera* camera)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
	}
	//速力処理更新
	update_velocity(elapsed_time, position);
}

//魔法弾射出行動
void Player::update_attack_bullet_state(float elapsed_time, Camera* camera)
{
	//敵の方向を向く
	DirectX::XMFLOAT3 target_vec = camera->get_lock_on_target() - position;
	target_vec.y = 0;
	Turn(elapsed_time, target_vec, chara_param.turn_speed * 2, orientation);
	if (model->is_end_animation())
	{
		transition_idle_state();
	}
	//速力処理更新
	update_velocity(elapsed_time, position);
}

//
void Player::update_attack_slash_up_state(float elapsed_time, Camera* camera)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
	}
	//速力処理更新
	update_velocity(elapsed_time, position);
}

void Player::update_r_attack_forward_state(float elapsed_time, Camera* camera)
{
	//*************************************//
	//	ルートモーションに関する更新	   //
	//*************************************//
	root_motion_manual(Math::get_posture_forward(orientation), 1.5f * elapsed_time);
	//速力処理更新
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

	//速力処理更新
	update_velocity(elapsed_time, position);
}
void Player::update_r_attack_combo1_state(float elapsed_time, Camera* camera)
{
	//*************************************//
	//			攻撃判定に関する更新	   //
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
		//攻撃判定ON
		attack_sword_param.is_attack = true;
		root_motion_manual(Math::get_posture_forward(orientation), param.floating_value);//ルートモーションじゃない手動入力

	}
	//*************************************//
	//			遷移に関する更新	   //
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


		//攻撃判定OFF
		attack_sword_param.is_attack = false;
	}
	else
	{
		//空中にいる場合少し浮遊
		if (floating())
		{
		}

	}
	input_move(elapsed_time, camera, param.floating_value, param.floating_value);
	//*************************************//
	//	ルートモーションに関する更新	   //
	//*************************************//
	//回避入力
	input_avoidance();
	//速力処理更新
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

		//攻撃判定ON
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
		//攻撃判定OFF
		attack_sword_param.is_attack = false;


	}
	//*************************************//
	//	ルートモーションに関する更新	   //
	//*************************************//
	root_motion_manual(Math::get_posture_forward(orientation), -10.0f * elapsed_time);
	//回避入力
	input_avoidance();
	//空中にいる場合少し浮遊
	floating();

	//速力処理更新
	update_velocity(elapsed_time, position);
}

void Player::update_r_attack_combo3_state(float elapsed_time, Camera* camera)
{
	DirectX::XMFLOAT3 sword_pos;
	DirectX::XMFLOAT4X4 bone_ori = {};
	model->fech_by_bone(transform, root, sword_pos, &bone_ori);
	DirectX::XMFLOAT3 slash_dir = sword->get_dir_sword_top();
	DirectX::XMFLOAT3 up = { bone_ori._11,bone_ori._12,bone_ori._13 };
	//一振り目の斬撃
	if (model->anime_param.frame_index == 4 / 2)
	{
		slash_efects[0]->play(sword->get_equipped_position());
		slash_efects[0]->rotate_base_axis(MeshEffect::AXIS::FORWARD, slash_dir);
		slash_efects[0]->rotate_base_axis(MeshEffect::AXIS::UP, up);
		slash_efects[0]->rot_speed.y = -param.sword_swing_speed;
		//攻撃判定ON
		attack_sword_param.is_attack = true;
		const float impulse_power = combo3_impulse1 * elapsed_time;
		add_impulse(Math::get_posture_forward(orientation) * impulse_power);
	}
	//二振り目の斬撃
	if (model->anime_param.frame_index == 30 / 2)
	{
		slash_efects[1]->play(sword->get_equipped_position());
		slash_efects[1]->rotate_base_axis(MeshEffect::AXIS::FORWARD, slash_dir);
		slash_efects[1]->rotate_base_axis(MeshEffect::AXIS::UP, up);
		slash_efects[1]->rot_speed.y = param.sword_swing_speed;
		const float impulse_power = combo3_impulse2 * elapsed_time;
		add_impulse(Math::get_posture_forward(orientation) * impulse_power);
	}
	//三振り目の斬撃
	if (model->anime_param.frame_index == 50 / 2)
	{
		slash_efects[2]->play(sword->get_equipped_position());
		slash_efects[2]->rotate_base_axis(MeshEffect::AXIS::FORWARD, slash_dir);
		slash_efects[2]->rotate_base_axis(MeshEffect::AXIS::UP, up);
		slash_efects[2]->rot_speed.y = -param.sword_swing_speed;
		velocity.x /= 5.0f;
		velocity.z /= 5.0f;
	}

	//3連撃目までは浮く
	if (model->anime_param.frame_index < 50 / 2)
	{
		//空中にいる場合少し浮遊
		floating();

	}


	if (model->anime_param.frame_index > 66 / 2)
	{
		if (model->anime_param.frame_index > 95 / 2 || model->is_end_animation())
		{
			transition_idle_state();
		}

		//攻撃判定OFF
		attack_sword_param.is_attack = false;
	}


	//回避入力
	input_avoidance();

	//速力処理更新
	update_velocity(elapsed_time, position);
}

void Player::update_r_attack_dodge_back_state(float elapsed_time, Camera* camera)
{
	//*************************************//
	//	ルートモーションに関する更新	   //
	//*************************************//
	float speed = model->root_defference_length_next_frame(root);
	velocity = Math::get_posture_forward(orientation) * (speed * add_root_speed);
}



