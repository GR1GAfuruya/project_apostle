#include "boss.h"

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


void Boss::transition_attack_state()
{
	act_update = &Boss::update_attack_state;
	model->play_animation(BossAnimation::ATTACK, false);
	state = State::ATTACK;
}

void Boss::transition_skill_1_state()
{
	act_update = &Boss::update_skill_1_state;
	model->play_animation(BossAnimation::SKILL_1, false);
	state = State::SKILL_1;
}

void Boss::transition_skill_2_start_state()
{
	act_update = &Boss::update_skill_2_start_state;
	model->play_animation(BossAnimation::SKILL_2_START, false);
	state = State::SKILL_2_START;

}

void Boss::transition_skill_2_loop_state()
{
	act_update = &Boss::update_skill_2_loop_state;
	model->play_animation(BossAnimation::SKILL_2_LOOP, true);
	state = State::SKILL_2_LOOP;
	//チャージエフェクト発生
	efc_charge_attack->play(position);
}

void Boss::transition_skill_2_end_state()
{
	act_update = &Boss::update_skill_2_end_state;
	model->play_animation(BossAnimation::SKILL_2_END, false);
	state = State::SKILL_2_END;
	//チャージエフェクトのチャージ状態をマックスに
	efc_charge_attack->set_charge_max_state();
}

void Boss::transition_skill_3_state()
{
	act_update = &Boss::update_skill_3_state;
	model->play_animation(BossAnimation::SKILL_3, false);
	state = State::SKILL_3;
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
	model->play_animation(BossAnimation::DAMAGE, false);
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
	model->play_animation(BossAnimation::DOWN, true);
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

void Boss::update_idle_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
	state_timer += elapsed_time;
	if (state_timer > 3.0)
	{
		
	}
}

void Boss::update_walk_state(Graphics& graphics, float elapsed_time, Stage* stage)
{

}

void Boss::update_run_state(Graphics& graphics, float elapsed_time, Stage* stage)
{

}

void Boss::update_attack_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
}

void Boss::update_skill_1_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
}

void Boss::update_skill_2_start_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
	if (model->is_end_animation())
	{
		transition_skill_2_loop_state();
	}
}

void Boss::update_skill_2_loop_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
	action_time += elapsed_time;
	if (action_time > 10)
	{
		action_time = 0;
		transition_skill_2_end_state();
	}
}

void Boss::update_skill_2_end_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
	if (model->is_end_animation())
	{
		transition_idle_state();
	}
}

void Boss::update_skill_3_state(Graphics& graphics, float elapsed_time, Stage* stage)
{

}

void Boss::update_air_bone_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
}

void Boss::update_damage_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
}

void Boss::update_dead_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
}

void Boss::update_die_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
}

void Boss::update_down_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
}

void Boss::update_downdead_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
}

void Boss::update_fall_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
}

void Boss::update_groggy_end_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
}

void Boss::update_groggy_loop_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
}

void Boss::update_groggy_start_state(Graphics& graphics, float elapsed_time, Stage* stage)
{
}

void Boss::update_stdand_state(Graphics& graphics, float elapsed_time, Stage* stage)
{

}

void Boss::update_stun_state(Graphics& graphics, float elapsed_time, Stage* stage)
{

}

