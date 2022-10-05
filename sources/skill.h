#pragma once
#include "graphics.h"
#include "imgui_include.h"
#include "primitive.h"
#include "user.h"
class Skill
{
public:
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================
	enum class CollisionType
	{
		SPHERE,
		CYLINDER,
		CUBE,
		CAPSULE,

	};


	//==============================================================
	// 
	// public関数
	// 
	//==============================================================

	Skill() {};
	virtual ~Skill() {};

	virtual void initialize(Graphics& graphics) {};
	virtual void initialize(Graphics& graphics, DirectX::XMFLOAT3 init_pos) {};
	virtual void initialize(Graphics& graphics, DirectX::XMFLOAT3 init_pos, DirectX::XMFLOAT3 target_pos) {};
	virtual void update(Graphics& graphics, float elapsed_time) = 0;
	virtual void render(Graphics& graphics) = 0;

	//スキルが終了したかどうか
	bool is_skill_end() { return skill_end_flag; };

	CollisionType get_collision_type() { return collision_type; }
	//当たり判定情報の取得
	Capsule& get_colider() { return attack_colider; }

	//スキルがヒットしたときに呼ぶ関数
	virtual void skill_hit() {};
	//デバッグGUI
	virtual void debug_gui(string str_id) = 0;
	//クールタイムの取得
	float get_cool_time() { return cool_time; }
	float get_power() { return power; }
	float get_invinsible_time() { return invinsible_time; }
protected:


	//==============================================================
	// 
	// 変数
	// 
	//==============================================================
	//当たり判定のタイプ
	CollisionType collision_type;
	//当たり判定（カプセル）
	Capsule attack_colider;
	//スキルの寿命
	float life_time = 0;
	//スキルが終了したか
	bool skill_end_flag = false;
	//スキルがヒットしたかどうか
	bool is_hit = false;
	//クールタイム
	float cool_time = 3.0f;
	//攻撃力
	float power;
	//対象に与える無敵時間
	float invinsible_time;
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================

};