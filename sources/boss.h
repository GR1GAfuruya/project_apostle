#pragma once
#include "charactor.h"
#include "skeletal_mesh.h"
#include "gpu_particle.h"
#include "slash.h"
#include "aura.h"
#include "boss_charge_attack.h"
#include "light.h"
class Boss :public Charactor
{
public:
	Boss(Graphics& graphics);
	~Boss() {};

	enum  BossAnimation
	{
		AIRBORNE,
		ATTACK,
		DAMAGE,
		DEAD,
		DIE,
		DOWN,
		DOWNDEAD,
		FALL,
		GROGGY_END,
		GROGGY_LOOP,
		GROGGY_START,
		IDLE,
		RUN,
		SKILL_1,
		SKILL_2_END,
		SKILL_2_LOOP,
		SKILL_2_START,
		SKILL_3,
		STAND,
		STUN,
		WALK

	};

	struct BodyCollision
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 position_end;
		float radius;
		float height;
	};
	
	void initialize();
	void update(Graphics& graphics, float elapsed_time, Stage* stage);
	//描画処理
	//ディファードでレンダリングするオブジェクト
	void render_d(Graphics& graphics, float elapsed_time);
	//フォワードレンダリングするオブジェクト
	void render_f(Graphics& graphics, float elapsed_time);
	//デバッグ用GUI描画
	void debug_gui();

	void calc_attack_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func);
private:
	//遷移
	void transition_air_bone_state();
	void transition_attack_state();
	void transition_damage_state();
	void transition_dead_state();
	void transition_die_state();
	void transition_down_state();
	void transition_downdead_state();
	void transition_fall_state();
	void transition_groggy_end_state();
	void transition_groggy_loop_state();
	void transitiond_groggy_start_state();
	void transition_idle_state();
	void transition_run_state();
	void transition_skill_1_state();
	void transition_skill_2_end_state();
	void transition_skill_2_loop_state();
	void transition_skill_2_start_state();
	void transition_skill_3_state();
	void transition_stand_state();
	void transition_stun_state();
	void transition_walk_state();

	//////アニメーションアップデート
	void update_air_bone_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_attack_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_damage_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_dead_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_die_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_down_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_downdead_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_fall_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_groggy_end_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_groggy_loop_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_groggy_start_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_idle_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_run_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_skill_1_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_skill_2_end_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_skill_2_loop_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_skill_2_start_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_skill_3_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_stdand_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_stun_state(Graphics& graphics, float elapsed_time, Stage* stage);
	void update_walk_state(Graphics& graphics, float elapsed_time, Stage* stage);

	//---------------<変数>-------------------//
	typedef void (Boss::* ActUpdate)(Graphics& graphics, float elapsed_time, Stage* stage);
	ActUpdate act_update = &Boss::update_idle_state;
	std::unique_ptr<SkeletalMesh> model;
	std::unique_ptr<ChargeAttack> efc_charge_attack;

	DirectX::XMFLOAT4X4 world;
	float action_time = 0;
	float move_speed = 30.0f;
	float turn_speed = DirectX::XMConvertToRadians(720);
	bool display_imgui = true;

public:
	BodyCollision boss_collision;

	AddDamageFunc damaged_function;
	int attack_power;
	int attack_add_invisible_time;
};