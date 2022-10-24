#pragma once
#include <vector>
#include "sprite.h"
#include "skill.h"
#include "damage_func.h"

class SkillLauncher
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
	SkillLauncher() :
		cool_time(0.0f),
		chantable(false),
		cool_time_attenuation_speed(1.0f) {};
	virtual ~SkillLauncher() {}

	//初期化
	void initialize(Graphics& graphics);
	//更新
	void update(Graphics& graphics, float elapsed_time);
	//描画
	void render(Graphics& graphics);

	//DebugGUI表示
	virtual void debug_gui()= 0;

	//※引数がそれぞれ異なるので継承先で実装
	////スキルの当たり判定
	//void skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func);
	////詠唱する
	//void launch();
	
	
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

	bool display_imgui = false;
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================

};