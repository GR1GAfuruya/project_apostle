#include "player.h"
#include "Operators.h"


void Player::transition_idle_state()
{
	p_update = &Player::update_idle_state;
	model->play_animation(PlayerAnimation::PLAYER_IDLE, true);
}

void Player::transition_attack_state()
{
	p_update = &Player::update_attack_state;
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

void Player::update_idle_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	if (input_move(elapsed_time, camera))
	{
		transition_move_state();
	}
	input_jump();

	//攻撃入力

	if (game_pad->get_button_down() & game_pad->BTN_X)
	{
		transition_attack_combo1_state();
	}

	//速力処理更新
	update_velocity(elapsed_time, position, stage);
}

void Player::update_attack_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	if (model->anime_param.frame_index > 43 / 2)
	{
		Attack(graphics, elapsed_time);
	}
	if (model->is_end_animation())
	{
		transition_idle_state();
	}

	//速力処理更新
	update_velocity(elapsed_time, position, stage);
}

void Player::update_attack_combo1_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	DirectX::XMFLOAT3 sword_pos, up;
	model->fech_by_bone(world, sword_hand, sword_pos, up);
	DirectX::XMVECTOR slash_dir_vec = get_posture_forward_vec(orientation);
	DirectX::XMVECTOR slash_slope_vec = DirectX::XMVectorMultiply(get_posture_right_vec(orientation), get_posture_up_vec(orientation));
	if (model->anime_param.frame_index == 20 / 2)
	{
		slash_efect->play(sword_pos + (up * 3.2f), DirectX::XMLoadFloat3(&up), slash_slope_vec, true);
	}
	if (model->anime_param.frame_index > 55 / 2)
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

	//速力処理更新
	update_velocity(elapsed_time, position, stage);
}

void Player::update_attack_combo2_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	DirectX::XMFLOAT3 sword_pos, up;
	model->fech_by_bone(world, sword_hand, sword_pos, up);
	DirectX::XMVECTOR slash_dir_vec = get_posture_forward_vec(orientation);
	DirectX::XMVECTOR slash_slope_vec = get_posture_up_vec(orientation);
	if (model->anime_param.frame_index == 21 / 2) slash_efect->play(sword_pos + (up * 1.2f), slash_dir_vec, slash_slope_vec, true);

	if (model->anime_param.frame_index > 66 / 2)
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
	//速力処理更新
	update_velocity(elapsed_time, position, stage);
}

void Player::update_attack_combo3_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	DirectX::XMFLOAT3 sword_pos, up;
	model->fech_by_bone(world, sword_hand, sword_pos, up);
	DirectX::XMVECTOR slash_dir_vec = get_posture_forward_vec(orientation);
	DirectX::XMVECTOR slash_slope_vec = get_posture_up_vec(orientation);
	//一振り目の斬撃
	if (model->anime_param.frame_index == 40 / 2)
		slash_efect->play(sword_pos, DirectX::XMLoadFloat3(&up), slash_slope_vec, false);
	//二振り目の斬撃
	if (model->anime_param.frame_index == 60 / 2)
		slash_efect->play(sword_pos, DirectX::XMLoadFloat3(&up), slash_dir_vec, true);
	//三振り目の斬撃
	if (model->anime_param.frame_index == 80 / 2)
		slash_efect->play(sword_pos, DirectX::XMLoadFloat3(&up), slash_slope_vec, false);

	if (model->anime_param.frame_index > 120 / 2)
	{
		if (model->is_end_animation())
		{
			transition_idle_state();
		}

		if (game_pad->get_button() & game_pad->BTN_X)
		{
			transition_attack_state();
		}

	}
	//速力処理更新
	update_velocity(elapsed_time, position, stage);
}

void Player::update_move_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	if (!input_move(elapsed_time, camera) && is_ground)
	{
		transition_idle_state();
	}

	input_jump();

	//攻撃入力
	if (game_pad->get_button() & game_pad->BTN_X)
	{
		transition_attack_combo1_state();
	}

	//速力処理更新
	update_velocity(elapsed_time, position, stage);
}

void Player::update_jump_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage)
{
	input_move(elapsed_time, camera);

	if (is_ground)
	{
		transition_idle_state();
	}

	//速力処理更新
	update_velocity(elapsed_time, position, stage);
}