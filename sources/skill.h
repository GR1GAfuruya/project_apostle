#pragma once
#include "graphics.h"
class Skill
{
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	Skill(Graphics& graphics) ;
	~Skill() {};

	void initialize(Graphics& graphics);
	void update(Graphics& graphics,float elapsed_time);
	void render(Graphics& graphics);

	bool is_skill_end() { return skill_end_flag; };

	void debug_gui(const char* str_id);
private:
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
	float life_time = 0;
	bool skill_end_flag = false;
	bool display_imgui;
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================
public:
	const float COOL_TIME = 3.0f;
};