#include "boss.h"
#include "noise.h"
///////////////////////////////////////////////////////
//
/*--------------------状態遷移------------------------*/
//
///////////////////////////////////////////////////////

void Boss::transition_idle_state()
{
	act_update = &Boss::update_idle_state;
	model->play_animation(BossAnimation::IDLE, true);
	state = State::IDLE;
}

void Boss::transition_run_state()
{
	act_update = &Boss::update_run_state;
	model->play_animation(BossAnimation::RUN, true);
	state = State::RUN;
}

void Boss::transition_walk_state()
{
	act_update = &Boss::update_walk_state;
	model->play_animation(BossAnimation::WALK, true);
	state = State::WALK;
}


void Boss::transition_normal_attack_state()
{
	act_update = &Boss::update_normal_attack_state;
	model->play_animation(BossAnimation::ATTACK, false);
	state = State::ATTACK;
}

void Boss::transition_skill_1_state()
{
	act_update = &Boss::update_skill_1_state;
	model->play_animation(BossAnimation::SKILL_1, false);
	//state = State::SKILL_1;
	state = State::ATTACK;

	attack_skill_1->chant(position, Math::get_posture_forward(orientation));

}

void Boss::transition_skill_2_start_state()
{
	act_update = &Boss::update_skill_2_start_state;
	model->play_animation(BossAnimation::SKILL_2_START, false, 0.5f);
	//state = State::SKILL_2_START;
	state = State::ATTACK;

}

void Boss::transition_skill_2_loop_state()
{
	act_update = &Boss::update_skill_2_loop_state;
	model->play_animation(BossAnimation::SKILL_2_LOOP, true, 0.5f);
	//state = State::SKILL_2_LOOP;
	state = State::ATTACK;
	//チャージエフェクト発生
	attack_skill_2->chant(position);
}

void Boss::transition_skill_2_end_state()
{
	act_update = &Boss::update_skill_2_end_state;
	model->play_animation(BossAnimation::SKILL_2_END, false);
	//state = State::SKILL_2_END;
	state = State::ATTACK;
	//チャージエフェクトのチャージ状態をマックスに
	attack_skill_2->set_charge_max_state();
}

void Boss::transition_skill_3_state()
{
	act_update = &Boss::update_skill_3_state;
	model->play_animation(BossAnimation::SKILL_3, false);
	//state = State::SKILL_3;
	state = State::ATTACK;
	attack_skill_3->chant(position, Math::get_posture_forward(orientation));
}


void Boss::transition_air_bone_state()
{
	act_update = &Boss::update_air_bone_state;
	model->play_animation(BossAnimation::AIRBORNE, false);
	state = State::AIRBORNE;
}


void Boss::transition_damage_state()
{
	act_update = &Boss::update_damage_state;
	model->play_animation(BossAnimation::DAMAGE, false, 0.1f);
	state = State::DAMAGE;
}

void Boss::transition_dead_state()
{
	act_update = &Boss::update_dead_state;
	model->play_animation(BossAnimation::DEAD, false);
	state = State::DEAD;
}

void Boss::transition_die_state()
{
	act_update = &Boss::update_die_state;
	model->play_animation(BossAnimation::DIE, false);
	state = State::DIE;
}

void Boss::transition_down_state()
{
	act_update = &Boss::update_down_state;
	model->play_animation(BossAnimation::DOWN, true, 0.1f);
	state = State::DOWN;
}

void Boss::transition_downdead_state()
{
	act_update = &Boss::update_downdead_state;
	model->play_animation(BossAnimation::DOWNDEAD, false);
	state = State::DOWNDEAD;
}

void Boss::transition_fall_state()
{
	act_update = &Boss::update_fall_state;
	model->play_animation(BossAnimation::FALL, false);
	state = State::FALL;
}


void Boss::transition_groggy_end_state()
{
	act_update = &Boss::update_groggy_end_state;
	model->play_animation(BossAnimation::GROGGY_END, false);
	state = State::GROGGY_END;
}

void Boss::transition_groggy_loop_state()
{
	act_update = &Boss::update_groggy_loop_state;
	model->play_animation(BossAnimation::GROGGY_LOOP, true);
	state = State::GROGGY_LOOP;
}

void Boss::transitiond_groggy_start_state()
{
	act_update = &Boss::update_groggy_start_state;
	model->play_animation(BossAnimation::GROGGY_START, false);
	state = State::GROGGY_START;
}

void Boss::transition_stand_state()
{
	act_update = &Boss::update_stdand_state;
	model->play_animation(BossAnimation::STAND, false);
	state = State::STAND;
}

void Boss::transition_stun_state()
{
	act_update = &Boss::update_stun_state;
	model->play_animation(BossAnimation::STUN, true);
	state = State::STUN;
}

///////////////////////////////////////////////////////
//
/*--------------------状態更新------------------------*/
//
///////////////////////////////////////////////////////

//---------------------------//
//			移動系			 //
//---------------------------//
void Boss::update_idle_state(float elapsed_time)
{
	state_timer += elapsed_time;
	if (state_timer > state_duration)
	{
		if (health < chara_param.max_health / 2)
		{
			//HP半分以下なら走る
			transition_run_state();
		}
		else
		{
			transition_walk_state();
		}

		state_timer = 0;
	}
	//速度更新
	update_velocity(elapsed_time, position);
}

void Boss::update_walk_state(float elapsed_time)
{
	//プレイヤー方向に歩く
	DirectX::XMFLOAT3 dir_target_vec = Math::calc_vector_AtoB_normalize(position, target_pos);
	Move(dir_target_vec.x, dir_target_vec.z, WALK_SPEED);
	Turn(elapsed_time, dir_target_vec, chara_param.turn_speed, orientation);

	//攻撃のルーチン
	attack_routine(elapsed_time);

	//速度更新
	update_velocity(elapsed_time, position);
}

void Boss::update_run_state(float elapsed_time)
{

	DirectX::XMFLOAT3 dir_target_vec = Math::calc_vector_AtoB_normalize(position, target_pos);
	Move(dir_target_vec.x, dir_target_vec.z, RUN_SPEED);
	Turn(elapsed_time, dir_target_vec, chara_param.turn_speed, orientation);

	//攻撃のルーチン
	attack_routine(elapsed_time);

	//速度更新
	update_velocity(elapsed_time, position);
}

//---------------------------//
//			攻撃系			 //
//---------------------------//
void Boss::update_normal_attack_state(float elapsed_time)
{
	sickle_attack_param.is_attack = true;
	if (model->is_end_animation())
	{
		transition_idle_state();
		state_duration = NORMAL_ATTACK_COOLTIME;
		sickle_attack_param.is_attack = false;
	}
	//速度更新
	update_velocity(elapsed_time, position);
}

void Boss::update_skill_1_state(float elapsed_time)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
		state_duration = SKILL1_COOLTIME;
	}
	//速度更新
	update_velocity(elapsed_time, position);
}

void Boss::update_skill_2_start_state(float elapsed_time)
{
	if (model->is_end_animation())
	{
		transition_skill_2_loop_state();
	}
}

void Boss::update_skill_2_loop_state(float elapsed_time)
{
	action_time += elapsed_time;
	if (action_time > 5)
	{
		action_time = 0;
		transition_skill_2_end_state();
	}
}

void Boss::update_skill_2_end_state(float elapsed_time)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
		state_duration = SKILL2_COOLTIME;
	}
}

void Boss::update_skill_3_state(float elapsed_time)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
		state_duration = SKILL3_COOLTIME;
	}
	//速度更新
	update_velocity(elapsed_time, position);
}

void Boss::attack_routine(float elapsed_time)
{
	float length_to_target = Math::calc_vector_AtoB_length(position, target_pos);
	if (length_to_target < ATTACK_ACTION_LENGTH)
	{
		//攻撃タイプ選択
		select_attack_type_short();
		return;
	}

	//プレイヤーが一定時間自分に近づかなかったら遠距離攻撃
	attack_responder_timer += elapsed_time;
	if (attack_responder_timer > ATTACK_RESPONDER_TIME)
	{
		select_attack_type_long();
		attack_responder_timer = 0;
	}
}

//---------------------------//
//			攻撃タイプ選択		//
//---------------------------//
void Boss::select_attack_type_short()
{
	int random = std::abs(static_cast<int>(Noise::instance().get_rnd())) % static_cast<int>(ATTACK_TYPE::MAX_NUM);
	//ランダムで攻撃方法を選択
	ATTACK_TYPE attack_type = static_cast<ATTACK_TYPE>(random);
	switch (attack_type)
	{
	case ATTACK_TYPE::NORMAL:
		transition_normal_attack_state();
		break;

	case ATTACK_TYPE::SKILL1:
		transition_skill_1_state();
		break;

	case ATTACK_TYPE::SKILL2:
		transition_skill_2_start_state();
		break;

	case ATTACK_TYPE::SKILL3:
		transition_skill_3_state();
		break;
	}
}

void Boss::select_attack_type_long()
{

	int random = std::abs(static_cast<int>(Noise::instance().get_rnd())) % static_cast<int>(ATTACK_TYPE::MAX_NUM);
	//ランダムで攻撃方法を選択
	ATTACK_TYPE attack_type = static_cast<ATTACK_TYPE>(random);
	//通常攻撃は除外し、遠距離攻撃のみ選択
	switch (attack_type)
	{
	case ATTACK_TYPE::SKILL1:
		transition_skill_1_state();
		break;

	case ATTACK_TYPE::SKILL2:
		transition_skill_2_start_state();
		break;

	case ATTACK_TYPE::SKILL3:
		transition_skill_3_state();
		break;
	}
}



//---------------------------//
//			ダウン系		//
//---------------------------//
void Boss::update_air_bone_state(float elapsed_time)
{
	if (model->is_end_animation())
	{
		transition_down_state();
	}
}

void Boss::update_damage_state(float elapsed_time)
{
	if (model->is_end_animation())
	{
		state_duration = DAMAGE_STUN_DURATION;;
		transition_idle_state();
	}
}

void Boss::update_dead_state(float elapsed_time)
{
}

void Boss::update_die_state(float elapsed_time)
{
}

void Boss::update_down_state(float elapsed_time)
{
}

void Boss::update_downdead_state(float elapsed_time)
{
}

void Boss::update_fall_state(float elapsed_time)
{
}

void Boss::update_groggy_end_state(float elapsed_time)
{
}

void Boss::update_groggy_loop_state(float elapsed_time)
{
}

void Boss::update_groggy_start_state(float elapsed_time)
{
}

void Boss::update_stdand_state(float elapsed_time)
{

}

void Boss::update_stun_state(float elapsed_time)
{

}

