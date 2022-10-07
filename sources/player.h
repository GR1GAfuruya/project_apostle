#pragma once
#include "device.h"
#include "camera.h"
#include "charactor.h"
#include "skeletal_mesh.h"
#include "gpu_particle.h"
#include "slash.h"
#include "tornado.h"
#include "sword.h"
#include "skill_manager.h"
#include "primitive.h"
//プレイヤー :final このクラスの継承ができないことを明示する
class Player final :public Charactor
{
public:
	Player(Graphics& graphics, Camera* camera);
	~Player()override;

	//初期化処理
	void initialize();
	//更新処理
	void update(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	//描画処理
	//ディファードでレンダリングするオブジェクト
	void render_d(Graphics& graphics, float elapsed_time, Camera* camera);
	//フォワードレンダリングするオブジェクト
	void render_f(Graphics& graphics, float elapsed_time, Camera* camera);

	void render_ui(Graphics& graphics, float elapsed_time);
	//デバッグ用GUI描画
	void debug_gui(Graphics& graphics);
	//プレイヤーの腰当たりの位置
	DirectX::XMFLOAT3 get_waist_position() { return DirectX::XMFLOAT3(position.x, position.y + height / 2, position.z); }
	//カメラがプレイヤーを見るときに注視するポイント
	DirectX::XMFLOAT3 get_gazing_point() { return DirectX::XMFLOAT3(position.x, position.y + (height + 3), position.z); }

	//プレイヤーのコリジョンと敵の当たり判定
	void calc_collision_vs_enemy(Capsule capsule_collider, float colider_height);

	//プレイヤーの攻撃と敵の当たり判定
	void calc_attack_vs_enemy(Capsule collider, AddDamageFunc damaged_func);

	//スキルとと当たり判定
	void judge_skill_collision(Capsule object_colider, AddDamageFunc damaged_func);
private:
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================
	//アニメーション
	enum  PlayerAnimation
	{
		PLAYER_IDLE,
		PLAYER_WALK,
		PLAYER_ROLL,
		PLAYER_ATK_AIR,
		PLAYER_SUPPORT_MAGIC,
		PLAYER_ATK_LOW,
		PLAYER_JUMP,
		PLAYER_ATK_COMBO1,
		PLAYER_ATK_COMBO2,
		PLAYER_ATK_COMBO3,
		PLAYER_ATK_PULL,
		PLAYER_ATK_BULLET,
		PLAYER_DAMAGE_FRONT,
	};

	enum class State
	{
		IDLE,
		MOVE,
		ROLL,
		JUMP,
		FALL,
		LANDING,
		FRONT_DAMAGE,

	};



	struct AttackParam
	{
		Capsule collision;
		bool is_attack;//攻撃中かどうか
		float power;//攻撃力
		float invinsible_time;//攻撃対象に課す無敵時間
	};
	//--------------------------------------------------------------
	//  定数
	//--------------------------------------------------------------
	
	 //攻撃1撃目の猶予時間
	static constexpr float ATTACK_TYPE1_MAX_TIME = 0.3f;
	//攻撃2撃目の猶予時間
	static constexpr float ATTACK_TYPE2_MAX_TIME = 0.2f;
	//攻撃3撃目の猶予時間
	static constexpr float ATTACK_TYPE3_MAX_TIME = 0.2f;



private:
	//==============================================================
	// 
	// private関数
	// 
	//==============================================================

	//遷移
	void transition_idle_state();
	void transition_attack_combo1_state();
	void transition_attack_combo2_state();
	void transition_attack_combo3_state();
	void transition_attack_combo4_state();
	void transition_attack_pull_state();
	void transition_attack_bullet_state();
	void transition_move_state();
	void transition_jump_state();
	void transition_avoidance_state();
	void transition_support_magic_state();
	void transition_damage_front_state();


	//////アニメーションアップデート
	void update_idle_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_combo1_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_combo2_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_combo3_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_combo4_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_pull_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_bullet_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_move_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_jump_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_avoidance_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_support_magic_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_damage_front_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);

	typedef void (Player::* ActUpdate)(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	//
	void Attack(Graphics& graphics, float elapsed_time);

	void select_support_skill();

	//プレイヤーの移動入力処理
	bool input_move(float elapsedTime, Camera* camera);
	const DirectX::XMFLOAT3 get_move_vec(Camera* camera) const;
	//ジャンプ入力処理
	void input_jump();
	//回避入力
	void input_avoidance();
	//サポートスキル詠唱入力
	void input_chant_support_skill(Graphics& graphics);
	//攻撃スキル詠唱入力
	void input_chant_attack_skill(Graphics& graphics);
	
	//着地したか
	void on_landing()override;

	void on_dead() override;

	void on_damaged() override;

	//==============================================================
	// 
	// 変数
	// 
	//==============================================================
	ActUpdate p_update = &Player::update_idle_state;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;

	State state;

	GamePad* game_pad;
	Mouse* mouse;

	// スケルタルメッシュの実体
	std::unique_ptr <SkeletalMesh> model;
	float move_speed = 30.0f;
	float turn_speed = DirectX::XMConvertToRadians(720);

	//ジャンプスピード
	float jump_speed = 35.0f;
	//現何回ジャンプしてるか
	int jump_count = 0;
	//ジャンプ可能回数
	int jump_limit = 1;
	float avoidance_speed = 50;
	bool display_player_imgui = true;
	/*-------攻撃関連--------------------------*/
	//攻撃時間
	float attack_time;
	//攻撃力
	int add_damage;

	std::unique_ptr<GPU_Particles> attack1;
	std::unique_ptr <SkillManager> skill_manager;
	std::unique_ptr<Slash> slash_efect;
	std::unique_ptr<Sword> sword;
	skeleton::bone sword_hand;
	skeleton::bone right_hand;

	//当たり判定用変数
	DirectX::XMFLOAT3 radius_aabb = { 5, 5, 5 };
	AttackParam attack_sword_param;
public:
	//ダメージを受けたときに呼ばれる *関数を呼ぶのはダメージを与えたオブジェクト
	AddDamageFunc damaged_function;
	Capsule collider;
};
