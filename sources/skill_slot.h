#pragma once
#include <vector>
#include "sprite.h"
#include "skill.h"
#include "damage_func.h"

class SkillSlot
{
public:
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================
	
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	SkillSlot() :
		cool_time(0.0f),
		chantable(false),
		cool_time_attenuation_speed(1.0f) {};
	virtual ~SkillSlot() {}

	//初期化
	void initialize(Graphics& graphics);
	//更新
	void update(Graphics& graphics, float elapsed_time);
	//描画
	void render(Graphics& graphics);

	//スキルの当たり判定
	void skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func);
	//DebugGUI表示
	virtual void debug_gui(string str_id);

	//詠唱する
	virtual void chant(Graphics& graphics) {};
	
protected:
	//==============================================================
	// 
	// 変数
	// 
	//==============================================================
	float cool_time;
	//詠唱可能か
	bool chantable;

	float cool_time_attenuation_speed;
	std::vector<std::unique_ptr<Skill>> skills;

	//==============================================================
	// 
	// 定数
	// 
	//==============================================================

};