#pragma once
#include "skill.h"
class Restrain : public Skill
{
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	Restrain(){}
	~Restrain() {}

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
};