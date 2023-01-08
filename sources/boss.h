#pragma once
#include "charactor.h"
#include "skeletal_mesh.h"
#include "gpu_particle.h"

#include "boss_charge_attack.h"
#include "light.h"
#include "primitive.h"
#include "boss_ui.h"
class Boss :public Charactor
{
private:
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================

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
	enum class State
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

	enum class ATTACK_TYPE
	{
		NORMAL,
		SKILL1,
		SKILL2,
		SKILL3,
		MAX_NUM
	};
	struct BodyCollision
	{
		Capsule capsule;
		float height;
	};


public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================

	Boss(Graphics& graphics);
	~Boss() {};
	
	//初期化
	void initialize();

	//更新
	void update(Graphics& graphics, float elapsed_time, Camera* camera);

	//描画処理
	//ディファードでレンダリングするオブジェクト
	void render_d(Graphics& graphics, float elapsed_time);
	//フォワードレンダリングするオブジェクト
	void render_f(Graphics& graphics, float elapsed_time);
	void render_ui(Graphics& graphics, float elapsed_time);

	//デバッグ用GUI描画
	void debug_gui();

	//プレイヤーの攻撃との当たり判定
	void calc_attack_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func);

	//攻撃対象の位置を取得
	void set_location_of_attack_target(DirectX::XMFLOAT3 target) { target_pos = target; }

	BodyCollision get_body_collision() { return boss_body_collision; }

	//カメラがプレイヤーを見るときに注視するポイント
	DirectX::XMFLOAT3 get_gazing_point() { return DirectX::XMFLOAT3(position.x, position.y + (height + 3), position.z); }
	//==============================================================
	// 
	// private関数
	// 
	//==============================================================

	/*--------------------状態遷移------------------------*/

	//---------------------------//
	//			移動系			 //
	//---------------------------//

	//待機状態に移行
	void transition_idle_state();
	//歩き状態に移行
	void transition_walk_state();
	//走り状態に移行
	void transition_run_state();

	//---------------------------//
	//			攻撃系			 //
	//---------------------------//
	
	//右腕横振り攻撃状態に移行
	void transition_attack_state();
	//手を振り上げ地面に打ち付ける攻撃状態に移行
	void transition_skill_1_state();
	//チャージ終了状態に移行
	void transition_skill_2_end_state();
	//チャージ中状態に移行
	void transition_skill_2_loop_state();
	//チャージ開始状態に移行
	void transition_skill_2_start_state();
	//少し溜め右腕を振り払う攻撃状態に移行
	void transition_skill_3_state();

	//---------------------------//
	//			ダウン系		 //
	//---------------------------//
	
	//空中吹き飛び状態に移行
	void transition_air_bone_state();
	//ダメージ怯み状態に移行
	void transition_damage_state();
	//死亡中（不動）状態に移行
	void transition_dead_state();
	//死亡状態に移行
	void transition_die_state();
	//倒れながら呼吸状態に移行
	void transition_down_state();
	//ダウン状態（不動）に移行
	void transition_downdead_state();
	//空中に吹き飛ばされた状態から落下状態に移行
	void transition_fall_state();
	//地面に手をつきダウン（初期）状態に移行
	void transitiond_groggy_start_state();
	//地面に手をつきダウン（ループ）状態に移行
	void transition_groggy_loop_state();
	//地面に手をつきダウンから起き上がり状態に移行
	void transition_groggy_end_state();
	//倒れた状態から起き上がる状態に移行
	void transition_stand_state();
	//立ったままスタン状態に移行
	void transition_stun_state();


	/*---------------状態更新------------------------*/

	//---------------------------//
	//			移動系			 //
	//---------------------------//
	
	//待機状態更新
	void update_idle_state(Graphics& graphics, float elapsed_time);
	//歩き状態更新
	void update_walk_state(Graphics& graphics, float elapsed_time);
	//走り状態更新
	void update_run_state(Graphics& graphics, float elapsed_time);


	//---------------------------//
	//			攻撃系			 //
	//---------------------------//
	
	//右腕横振り攻撃状態更新
	void update_attack_state(Graphics& graphics, float elapsed_time);
	//手を振り上げ地面に打ち付ける攻撃状態更新
	void update_skill_1_state(Graphics& graphics, float elapsed_time);
	//チャージ終了状態更新
	void update_skill_2_end_state(Graphics& graphics, float elapsed_time);
	//チャージ中状態更新
	void update_skill_2_loop_state(Graphics& graphics, float elapsed_time);
	//チャージ開始状態更新
	void update_skill_2_start_state(Graphics& graphics, float elapsed_time);
	//少し溜め右腕を振り払う攻撃状態更新
	void update_skill_3_state(Graphics& graphics, float elapsed_time);

	//攻撃方法選択
	void select_attack_type();

	//---------------------------//
	//			ダウン系		 //
	//---------------------------//
	//空中吹き飛び状態更新
	void update_air_bone_state(Graphics& graphics, float elapsed_time);
	//ダメージ怯み状態更新
	void update_damage_state(Graphics& graphics, float elapsed_time);
	//死亡中（不動）状態更新
	void update_dead_state(Graphics& graphics, float elapsed_time);
	//死亡状態更新
	void update_die_state(Graphics& graphics, float elapsed_time);
	//倒れながら呼吸状態更新
	void update_down_state(Graphics& graphics, float elapsed_time);
	//ダウン状態（不動）状態更新
	void update_downdead_state(Graphics& graphics, float elapsed_time);
	//空中に吹き飛ばされた状態から落下状態更新
	void update_fall_state(Graphics& graphics, float elapsed_time);
	//地面に手をつきダウン（初期）状態更新
	void update_groggy_start_state(Graphics& graphics, float elapsed_time);
	//地面に手をつきダウン（ループ）状態更新
	void update_groggy_loop_state(Graphics& graphics, float elapsed_time);
	//地面に手をつきダウンから起き上がり状態更新
	void update_groggy_end_state(Graphics& graphics, float elapsed_time);
	//倒れた状態から起き上がる状態更新
	void update_stdand_state(Graphics& graphics, float elapsed_time);
	//立ったままスタン状態更新
	void update_stun_state(Graphics& graphics, float elapsed_time);

	//ー－－－
	void on_dead() override;
	void on_damaged(WINCE_TYPE type) override;

	//==============================================================
	// 
	// 変数
	// 
	//==============================================================
	typedef void (Boss::* ActUpdate)(Graphics& graphics, float elapsed_time);
	ActUpdate act_update = &Boss::update_idle_state;
	std::unique_ptr<SkeletalMesh> model;
	std::unique_ptr<ChargeAttack> efc_charge_attack;
	std::unique_ptr<BossUi> ui;

	//鎌腕のボーン
	skeleton::bone sickle_hand;

	float action_time = 0;
	float move_speed = 30.0f;
	float turn_speed = 5.0f;
	bool display_imgui = false;

	//ステートのタイマー
	float state_timer;

	float state_duration;
	//攻撃対象
	DirectX::XMFLOAT3 target_pos;

	//
	State state;
	
	AttackParam sickle_attack_param;
	BodyCollision boss_body_collision;

#if _DEBUG
	bool is_update = true;
	bool is_render = true;
#endif
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================
	//歩くスピード
	float WALK_SPEED = 15;
	//走るスピード
	float RUN_SPEED = 30;
	//通常攻撃のクールタイム
	float ATTACK_ACTION_LENGTH = 17;
	//通常攻撃のクールタイム
	float NORMAL_ATTACK_COOLTIME = 1;
	//
	float SKILL1_COOLTIME = 2;
	//
	float SKILL2_COOLTIME = 3;
	//
	float SKILL3_COOLTIME = 2;
	//ダメージを受けたときのスタン時間
	float DAMAGE_STUN_DURATION = 0.7f;
public:

	AddDamageFunc damaged_function;
};