#include "player.h"
#include "Operators.h"

///////////////////////////////////////////////////////
//
/*--------------------ó‘Ô‘JˆÚ------------------------*/
//
///////////////////////////////////////////////////////
void Player::transition_idle_state()
{
	p_update = &Player::update_idle_state;
	model->play_animation(PlayerAnimation::PLAYER_IDLE, true);
}

void Player::transition_attack_combo1_state()
{
	p_update = &Player::update_attack_combo1_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_COMBO1, false, 0.1f);
	//UŒ‚ƒpƒ‰ƒ[ƒ^[Ý’è
	attack_sword_param.power = 10;
	attack_sword_param.invinsible_time = 0.5f;
}

void Player::transition_attack_combo2_state()
{
	p_update = &Player::update_attack_combo2_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_COMBO2, false, 0.1f);
	//UŒ‚ƒpƒ‰ƒ[ƒ^[Ý’è
	attack_sword_param.power = 13;
	attack_sword_param.invinsible_time = 0.5f;
}

void Player::transition_attack_combo3_state()
{
	p_update = &Player::update_attack_combo3_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_COMBO3, false, 0.1f);
	//UŒ‚ƒpƒ‰ƒ[ƒ^[Ý’è
	attack_sword_param.power = 15;
	attack_sword_param.invinsible_time = 0.2f;
}

void Player::transition_attack_combo4_state()
{
	p_update = &Player::update_attack_combo4_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_AIR, false);

}

void Player::transition_attack_pull_state()
{
	p_update = &Player::update_attack_pull_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_PULL, false, 0.1f);

}

void Player::transition_attack_bullet_state()
{
	p_update = &Player::update_attack_bullet_state;
	model->play_animation(PlayerAnimation::PLAYER_ATK_BULLET, false, 0.1f);

}

void Player::transition_move_state()
{
	p_update = &Player::update_move_state;
	model->play_animation(PlayerAnimation::PLAYER_WALK, true);
}

void Player::transition_jump_state()
{
	p_update = &Player::update_jump_state;
	model->play_animation(PlayerAnimation::PLAYER_JUMP, false, 0.1f);
}

void Player::transition_avoidance_state()
{
	p_update = &Player::update_avoidance_state;
	model->play_animation(PlayerAnimation::PLAYER_ROLL, false, 0.1f);
}

void Player::transition_support_magic_state()
{
	p_update = &Player::update_support_magic_state;
	model->play_animation(PlayerAnimation::PLAYER_SUPPORT_MAGIC, false);
}


void Player::transition_damage_front_state()
{
	p_update = &Player::update_damage_front_state;
	model->play_animation(PlayerAnimation::PLAYER_DAMAGE_FRONT, false);

}

///////////////////////////////////////////////////////
//
/*--------------------ó‘ÔXV------------------------*/
//
///////////////////////////////////////////////////////
void Player::update_idle_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	if (input_move(elapsed_time, camera))
	{
		transition_move_state();
	}
	//ƒWƒƒƒ“ƒv“ü—Í
	input_jump();
	//‰ñ”ð“ü—Í
	input_avoidance();

	//UŒ‚“ü—Í
	if (game_pad->get_button_down() & game_pad->BTN_X)
	{
		transition_attack_combo1_state();
	}

	//‘¬—Íˆ—XV
	update_velocity(elapsed_time, position, stage);
}

void Player::update_attack_combo1_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	DirectX::XMFLOAT3 sword_pos;
	DirectX::XMFLOAT4X4 bone_ori = {};
	model->fech_by_bone(transform, sword_hand, sword_pos, &bone_ori);
	DirectX::XMFLOAT3 slash_dir = sword->get_dir_sword_top();
	DirectX::XMFLOAT3 up = { bone_ori._11,bone_ori._12,bone_ori._13 };
	if (model->anime_param.frame_index == 20 / 2)
	{
		slash_efect->play(sword->get_equipped_position());
		slash_efect->set_life_span(0.2f);
		slash_efect->rotate_base_axis(MeshEffect::AXIS::FORWARD, slash_dir);
		slash_efect->rotate_base_axis(MeshEffect::AXIS::UP, up);
		slash_efect->rot_speed.y = -sword_swing_speed;
		//UŒ‚”»’èON
		attack_sword_param.is_attack = true;
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
		//UŒ‚”»’èOFF
		attack_sword_param.is_attack = false;
	}
	//‰ñ”ð“ü—Í
	input_avoidance();
	//‘¬—Íˆ—XV
	update_velocity(elapsed_time, position, stage);
}

void Player::update_attack_combo2_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	DirectX::XMFLOAT3 sword_pos;
	DirectX::XMFLOAT4X4 bone_ori = {};
	model->fech_by_bone(transform, sword_hand, sword_pos, &bone_ori);
	DirectX::XMFLOAT3 slash_dir = sword->get_dir_sword_top();
	if (model->anime_param.frame_index == 18 / 2)
	{
		slash_efect->play(sword->get_equipped_position());
		slash_efect->set_life_span(0.15f);
		slash_efect->rotate_base_axis(MeshEffect::AXIS::FORWARD, slash_dir);
		slash_efect->rot_speed.y = sword_swing_speed;

		//UŒ‚”»’èON
		attack_sword_param.is_attack = true;

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
		//UŒ‚”»’èOFF
		attack_sword_param.is_attack = false;


	}
	//‰ñ”ð“ü—Í
	input_avoidance();
	//‘¬—Íˆ—XV
	update_velocity(elapsed_time, position, stage);
}

void Player::update_attack_combo3_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	DirectX::XMFLOAT3 sword_pos;
	DirectX::XMFLOAT4X4 bone_ori = {};
	model->fech_by_bone(transform, sword_hand, sword_pos, &bone_ori);
	DirectX::XMFLOAT3 slash_dir = sword->get_dir_sword_top();
	DirectX::XMFLOAT3 up = { bone_ori._11,bone_ori._12,bone_ori._13 };
	//ˆêU‚è–Ú‚ÌŽaŒ‚
	if (model->anime_param.frame_index == 20 / 2)
	{
		slash_efect->play(sword->get_equipped_position());
		slash_efect->set_life_span(0.1f);
		slash_efect->rotate_base_axis(MeshEffect::AXIS::FORWARD, slash_dir);
		slash_efect->rotate_base_axis(MeshEffect::AXIS::UP, up);
		slash_efect->rot_speed.y = -sword_swing_speed;
		//UŒ‚”»’èON
		attack_sword_param.is_attack = true;

	}
	//“ñU‚è–Ú‚ÌŽaŒ‚
	if (model->anime_param.frame_index == 30 / 2)
	{
		slash_efect->play(sword->get_equipped_position());
		slash_efect->set_life_span(0.2f);
		slash_efect->rotate_base_axis(MeshEffect::AXIS::FORWARD, slash_dir);
		slash_efect->rotate_base_axis(MeshEffect::AXIS::UP, up);
		slash_efect->rot_speed.y = sword_swing_speed;
	}
	//ŽOU‚è–Ú‚ÌŽaŒ‚
	if (model->anime_param.frame_index == 55 / 2)
	{
		slash_efect->play(sword->get_equipped_position());
		slash_efect->set_life_span(0.2f);
		slash_efect->rotate_base_axis(MeshEffect::AXIS::FORWARD, slash_dir);
		slash_efect->rotate_base_axis(MeshEffect::AXIS::UP, up);
		slash_efect->rot_speed.y = -sword_swing_speed;
	}

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
		//UŒ‚”»’èOFF
		attack_sword_param.is_attack = false;


	}
	//‰ñ”ð“ü—Í
	input_avoidance();
	//‘¬—Íˆ—XV
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

	//‘¬—Íˆ—XV
	update_velocity(elapsed_time, position, stage);
}

void Player::update_attack_pull_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
}

void Player::update_move_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	if (!input_move(elapsed_time, camera) && is_ground)
	{
		transition_idle_state();
	}

	input_jump();
	input_avoidance();

	//UŒ‚“ü—Í
	if (game_pad->get_button() & game_pad->BTN_X)
	{
		transition_attack_combo1_state();
	}

	//‘¬—Íˆ—XV
	update_velocity(elapsed_time, position, stage);
}

void Player::update_jump_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	input_move(elapsed_time, camera);

	if (is_ground)
	{
		transition_idle_state();
	}

	//‘¬—Íˆ—XV
	update_velocity(elapsed_time, position, stage);
}

//‰ñ”ð
void Player::update_avoidance_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	//™X‚É‘¬“x‚ð—Ž‚Æ‚µ‚Ä‚¢‚­
	velocity.x /= 2 ;
	velocity.z /= 2 ;
	//‘¬—Íˆ—XV
	update_velocity(elapsed_time, position, stage);
	if (model->anime_param.frame_index > 33 / 2)
	{
		//’n–Ê‚É‘«‚ª‚Â‚¢‚½ƒtƒŒ[ƒ€‚©‚ç‚Í‚³‚ç‚É‘¬“x—Ž‚Æ‚·
		velocity.x /= 2;
		velocity.z /= 2;
	}
	else
	{
		//Œü‚¢‚Ä‚¢‚é•ûŒü‚É‘¬“x‚ð‘«‚·
		velocity.x += (Math::get_posture_forward(orientation) * (avoidance_speed)).x ;
		velocity.z += (Math::get_posture_forward(orientation) * (avoidance_speed)).z ;

	}

	//‘JˆÚˆ—
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

void Player::update_support_magic_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
	}
}

void Player::update_attack_bullet_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
	}
}

void Player::update_damage_front_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
	}
}
