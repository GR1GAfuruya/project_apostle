#pragma once
#include "graphics.h"
#include "imgui_include.h"
#include "primitive.h"
#include "mesh_effect.h"
#include "light.h"
#include "user.h"
#include "damage_func.h"
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

	virtual void initialize() {};
	virtual void update(float elapsed_time) = 0;
	virtual void render(Camera* camera) = 0;

	//スキルが終了したかどうか
	bool is_skill_end() { return skill_end_flag; };
	//スキルがヒットしているか
	bool is_skill_hit() { return is_hit; };

	CollisionType get_collision_type() { return collision_type; }
	//当たり判定情報の取得
	Capsule& get_colider() { return attack_colider; }

	//ポジションのゲッター
	const DirectX::XMFLOAT3 get_position() { return position; }
	//スキルがヒットしたときに呼ぶ関数
	virtual void skill_hit() {};
	//デバッグGUI
	virtual void debug_gui(string str_id) = 0;
	void set_is_skill_hit(bool h) { is_hit = h; }
protected:


	//==============================================================
	// 
	// 変数
	// 
	//==============================================================
	//当たり判定のタイプ
	CollisionType collision_type{};
	//当たり判定（カプセル）
	Capsule attack_colider{};
	//スキルの寿命
	float life_time = 0;
	//スキルの寿命
	float skill_duration = 0;

	//スキルが終了したか
	bool skill_end_flag = false;
	//スキルがヒットしたかどうか
	bool is_hit = false;
	//位置
	DirectX::XMFLOAT3 position{};
	//速力
	DirectX::XMFLOAT3 velocity{};
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================

};