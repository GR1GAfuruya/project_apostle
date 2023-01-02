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
		reduction_rate(1.0f) {};
	virtual ~SkillLauncher() {}

	//初期化
	virtual void initialize(Graphics& graphics);
	//更新
	virtual void update(Graphics& graphics, float elapsed_time);
	//描画
	virtual void render(Graphics& graphics);
	//スキルクールタイム短縮
	void cool_time_reduction();
	//DebugGUI表示
	virtual void debug_gui()= 0;
	//クールタイム取得
	float get_cool_time() { return cool_time ; }
	//クールタイムのパーセント取得
	float get_cool_time_percent() { return cool_time <= 0 ? 0.0f :1.0f - ( cool_time / max_cool_time); }

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
	float cool_time = 0;
	float max_cool_time = 0;
	//詠唱可能か
	bool chantable;
	//クールタイム短縮率
	float reduction_rate;
	std::vector<std::unique_ptr<Skill>> skills;

	bool display_imgui = false;
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================

};