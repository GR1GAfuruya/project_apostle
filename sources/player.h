#pragma once
#include "device.h"
#include "camera.h"
#include "charactor.h"
#include "skeletal_mesh.h"
#include "gpu_particle.h"
#include "mesh_effect.h"
#include "sword.h"
#include "skill_manager.h"
#include "primitive.h"
#include "player_ui.h"
#include <cereal/cereal.hpp>
//プレイヤー :final このクラスの継承ができないことを明示する
class Player final :public Charactor
{
public:
	Player(Graphics& graphics, Camera* camera);
	~Player()override;

	//初期化処理
	void initialize();
	//更新処理
	void update(Graphics& graphics, float elapsed_time, Camera* camera);
	//描画処理
	//ディファードでレンダリングするオブジェクト
	void render_d(Graphics& graphics, float elapsed_time, Camera* camera);
	//フォワードレンダリングするオブジェクト
	void render_f(Graphics& graphics, float elapsed_time, Camera* camera);
	//UI描画
	void render_ui(Graphics& graphics, float elapsed_time);
	//デバッグ用GUI描画
	void debug_gui(Graphics& graphics);
	//プレイヤーの腰当たりの位置
	DirectX::XMFLOAT3 get_waist_position() { return DirectX::XMFLOAT3(position.x, position.y + chara_param.height / 2, position.z); }
	//カメラがプレイヤーを見るときに注視するポイント
	DirectX::XMFLOAT3 get_gazing_point() { return DirectX::XMFLOAT3(position.x, position.y + (chara_param.height + 3), position.z); }

	//プレイヤーのコリジョンと敵の当たり判定
	void calc_collision_vs_enemy(Capsule capsule_collider, float colider_height);

	//プレイヤーの攻撃と敵の当たり判定
	void calc_attack_vs_enemy(Capsule collider, AddDamageFunc damaged_func, Camera* camera);

	//スキルと敵の当たり判定
	void judge_skill_collision(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera);
private:
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================
	//アニメーション
	enum  PlayerAnimation
	{
		PLAYER_IDLE,//待機
		PLAYER_RUN,//走り
		PLAYER_ROLL,//回避
		PLAYER_JUMP,//ジャンプ
		PLAYER_DAMAGE_FRONT,//前から被ダメ
		PLAYER_ATK_SPRING_SLASH,//前回転切り
		PLAYER_PULL_SLASH,//敵を引き付けて斬る
		PLAYER_ATK_GROUND,//地面に手を付けて口寄せみたいな
		PLAYER_MAGIC_BUFF,//バフ
		PLAYER_MAGIC_SLASH_UP,//空中に巻き上げ斬る
		PLAYER_MAGIC_BULLET,//小さい魔法弾打つような
		PLAYER_ATK_FORWARD_SLASH,//前進斬り
		PLAYER_ATK_AIR,//ジャンプして地面に魔法うつ
		PLAYER_ATK_COMBO1,//コンボ2-1
		PLAYER_ATK_COMBO2,//コンボ2-2
		PLAYER_ATK_COMBO3,//コンボ2-3
		PLAYER_ATK_DODGE_BACK,//後方に回避しながら魔法
	};

	//ステート
	enum class State
	{
		IDLE,
		MOVE,
		ROLL,
		JUMP,
		LANDING,
		FRONT_DAMAGE,
		NORMAL_ATTACK,
		SKILL,

	};

	struct PlayerParam
	{
		//基底クラスのパラメーター
		CharactorParam chara_init_param;
		//ジャンプスピード
		float jump_speed;
		//回避速度
		float avoidance_speed = 50;
		//浮遊度
		float floating_value = 10.0f;
		//剣エフェクトの速度
		float sword_swing_speed = 1500.0f;
		//コンボ1攻撃のパラメーター
		AttackParam combo_1;
		//コンボ2のパラメーター
		AttackParam combo_2;
		//コンボ3のパラメーター
		AttackParam combo_3;


		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				cereal::make_nvp("chara_param", chara_init_param),
				cereal::make_nvp("jump_speed", jump_speed),
				cereal::make_nvp("avoidance_speed", avoidance_speed),
				cereal::make_nvp("floating_value", floating_value),
				cereal::make_nvp("sword_swing_speed", sword_swing_speed),
				cereal::make_nvp("attack_combo_1", combo_1),
				cereal::make_nvp("attack_combo_2", combo_2),
				cereal::make_nvp("attack_combo_3", combo_3)
			);
		}
	};


private:
	//==============================================================
	// 
	// private関数
	// 
	//==============================================================

	//**********遷移*************//
	
	void transition_idle_state();//待機
	void transition_move_state();//走り
	void transition_avoidance_state();//回避
	void transition_jump_state();//ジャンプ
	void transition_damage_front_state();//前から被ダメ
	void transition_r_attack_spring_slash_state();//前回転切り
	void transition_attack_pull_slash_state();//敵を引き付けて斬る
	void transition_attack_ground_state();//地面に手を付けて口寄せみたいな
	void transition_magic_buff_state();//バフ
	void transition_attack_bullet_state();//小さい魔法弾打つような
	void transition_attack_slash_up_state();//空中に巻き上げ斬る
	void transition_r_attack_forward_state();//前進斬り
	void transition_attack_air_state();//ジャンプして地面に魔法うつ
	void transition_r_attack_combo1_state();//コンボ2-1
	void transition_r_attack_combo2_state();//コンボ2-2
	void transition_r_attack_combo3_state();//コンボ2-3
	void transition_r_attack_dodge_back_state();//後方に回避しながら魔法


	//********各ステートのアップデート**********//r_はルートモーション付き
	void update_idle_state(Graphics& graphics, float elapsed_time, Camera* camera);
	void update_move_state(Graphics& graphics, float elapsed_time, Camera* camera);//走り
	void update_avoidance_state(Graphics& graphics, float elapsed_time, Camera* camera);//回避
	void update_jump_state(Graphics& graphics, float elapsed_time, Camera* camera);//ジャンプ
	void update_damage_front_state(Graphics& graphics, float elapsed_time, Camera* camera);//前から被ダメ
	void update_r_attack_spring_slash_state(Graphics& graphics, float elapsed_time, Camera* camera);//前回転切り
	void update_attack_pull_slash_state(Graphics& graphics, float elapsed_time, Camera* camera);//敵を引き付けて斬る
	void update_attack_ground_state(Graphics& graphics, float elapsed_time, Camera* camera);//地面に手を付けて口寄せみたいな
	void update_magic_buff_state(Graphics& graphics, float elapsed_time, Camera* camera);//バフ
	void update_attack_bullet_state(Graphics& graphics, float elapsed_time, Camera* camera);//空中に巻き上げ斬る
	void update_attack_slash_up_state(Graphics& graphics, float elapsed_time, Camera* camera);//小さい魔法弾打つような
	void update_r_attack_forward_state(Graphics& graphics, float elapsed_time, Camera* camera);//前進斬り
	void update_attack_air_state(Graphics& graphics, float elapsed_time, Camera* camera);//ジャンプして地面に魔法うつ
	void update_r_attack_combo1_state(Graphics& graphics, float elapsed_time, Camera* camera);//コンボ2-1
	void update_r_attack_combo2_state(Graphics& graphics, float elapsed_time, Camera* camera);//コンボ2-2
	void update_r_attack_combo3_state(Graphics& graphics, float elapsed_time, Camera* camera);//コンボ2-3
	void update_r_attack_dodge_back_state(Graphics& graphics, float elapsed_time, Camera* camera);//後方に回避しながら魔法

	//更新関数の関数ポインタの定義
	typedef void (Player::* ActUpdate)(Graphics& graphics, float elapsed_time, Camera* camera);

	//プレイヤーの移動入力処理
	bool input_move(float elapsedTime, Camera* camera);

	//制限付きの移動（攻撃中などの移動入力）
	bool input_move(float elapsedTime, Camera* camera,float restriction_move, float restriction_turn);
	const DirectX::XMFLOAT3 get_move_vec(Camera* camera) const;
	//ジャンプ入力処理
	void input_jump();
	//回避入力
	void input_avoidance();
	//サポートスキル詠唱入力
	void input_chant_support_skill(Graphics& graphics, Camera* camera);
	//攻撃スキル詠唱入力
	void input_chant_attack_skill(Graphics& graphics, Camera* camera);
	
	//着地したか
	void on_landing()override;
	//死亡したときの処理
	void on_dead() override;
	//ダメージを受けた時の処理
	void on_damaged(WINCE_TYPE type) override;
	//ルートモーション
	void root_motion(DirectX::XMFLOAT3 dir, float speed);
	void root_motion_manual(DirectX::XMFLOAT3 dir, float speed);
	//少し浮遊する
	bool floating();

	//----------<ファイル>------------//
	void load_data_file();
	void save_data_file();
	const char* file_path = "./resources/Data/player_param.json";
	//==============================================================
	// 
	// 変数
	// 
	//==============================================================
	//関数ポインタの宣言
	ActUpdate p_update = &Player::update_idle_state;


	PlayerParam param;
	State state;

	GamePad* game_pad;
	Mouse* mouse;

	// スケルタルメッシュの実体
	std::unique_ptr <SkeletalMesh> model;

	//現何回ジャンプしてるか
	int jump_count = 0;
	//ジャンプ可能回数
	int jump_limit = 1;
	bool display_player_imgui = false;

	DirectX::XMFLOAT3 left_hand_pos;
	/*-------攻撃関連--------------------------*/
	//攻撃時間
	float attack_time;

	//剣攻撃のヒットエフェクト
	std::unique_ptr<GPU_Particles> slash_hit_particle;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> slash_hit_emit_cs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> slash_hit_update_cs;
	std::unique_ptr<MeshEffect> slash_efects[3];
	std::unique_ptr<MeshEffect> test_slash_hit;
	//スキルマネージャー
	std::unique_ptr <SkillManager> skill_manager;
	//剣
	std::unique_ptr<Sword> sword;
	//UI
	std::unique_ptr<PlayerUI> ui;

	//左手のボーン
	skeleton::bone left_hand;
	//右手のボーン
	skeleton::bone right_hand;
	//
	DirectX::XMFLOAT3 root_defference_velocity;
	//当たり判定用変数
	DirectX::XMFLOAT3 radius_aabb = { 5, 5, 5 };
	AttackParam attack_sword_param;
	//ルートモーション
	skeleton::bone root;
	DirectX::XMFLOAT3 root_motion_pos = {0,0,0};
	float add_root_speed = 1.1f;
	bool is_root_motion = false;

	
public:
	//ダメージを受けたときに呼ばれる *関数を呼ぶのはダメージを与えたオブジェクト
	AddDamageFunc damaged_function;
	Capsule collider;
};
