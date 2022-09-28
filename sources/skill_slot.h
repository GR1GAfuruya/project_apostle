#pragma once
#include <vector>
#include "sprite.h"
#include "skill.h"
#define SKILLTYPE SkillSlot::SkillType

class SkillSlot
{
public:
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================
	enum class SkillType
	{
		REGENERATE,
		PHYCICAL_UP,
		RESTRAINNT,
		MAGICBULLET,
		SPEARS_SEA
	};

public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	SkillSlot(Graphics& graphics, SkillType type);
	~SkillSlot() {}

	//初期化
	void initialize(Graphics& graphics);
	//更新
	void update(Graphics& graphics, float elapsed_time);
	//描画
	void render(Graphics& graphics);
	//DebugGUI表示
	void debug_gui(string str_id);

	//詠唱する
	void chant(Graphics& graphics);
	

	void entity_generation_by_type(Graphics& graphics, std::unique_ptr<Skill>& coffin ,SkillType type);
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

	SkillType skill_type;
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================

};