#pragma once
#include <vector>
#include "sprite.h"
#include "skill.h"
class SkillSlot
{
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	SkillSlot(Graphics& graphics) {};
	~SkillSlot() {}

	//初期化
	void initialize(Graphics& graphics);
	//更新
	void update(Graphics& graphics, float elapsed_time);
	//描画
	void render(Graphics& graphics);
	//DebugGUI表示
	void debug_gui();

	//詠唱する
	void chant(Graphics& graphics);
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================



	//==============================================================
	// 
	// 変数
	// 
	//==============================================================
	float cool_time;
	//詠唱可能か
	bool chantable;

	float cool_time_attenuation_speed;
	std::unique_ptr<Sprite> icon;
	std::vector<std::unique_ptr<Skill>> skill;

	//==============================================================
	// 
	// 定数
	// 
	//==============================================================

};