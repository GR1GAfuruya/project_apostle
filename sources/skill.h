#pragma once
#include "graphics.h"
#include "imgui_include.h"
#include "primitive.h"
#include "mesh_effect.h"
#include "light.h"
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
	virtual void update(Graphics& graphics, float elapsed_time) = 0;
	virtual void render(Graphics& graphics) = 0;

	//スキルが終了したかどうか
	bool is_skill_end() { return skill_end_flag; };

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
	float get_power() { return power; }
	float get_invinsible_time() { return invinsible_time; }
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
	//スキルが終了したか
	bool skill_end_flag = false;
	//スキルがヒットしたかどうか
	bool is_hit = false;
	//攻撃力
	float power = 0.0f;
	//対象に与える無敵時間
	float invinsible_time = 0.0f;
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