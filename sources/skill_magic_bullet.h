#pragma once
#include "skill.h"
class MagicBullet : public Skill
{
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	MagicBullet() {}
	~MagicBullet() {}

	void initialize(Graphics& graphics) override;
	void update(Graphics& graphics, float elapsed_time)override;
	void render(Graphics& graphics)override;

	bool is_skill_end() { return skill_end_flag; };

	void debug_gui(string str_id);
protected:
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


	//==============================================================
	// 
	// 定数
	// 
	//==============================================================
public:

};