#pragma once
#include "skill.h"
class SpearsSea : public Skill
{
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	SpearsSea(){}
	~SpearsSea() {}

	void initialize(Graphics& graphics) override;
	void update(Graphics& graphics, float elapsed_time)override;
	void render(Graphics& graphics)override;

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