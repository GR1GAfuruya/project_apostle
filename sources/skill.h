#pragma once
#include "graphics.h"
class Skill
{
public:
	//==============================================================
	// 
	// publicŠÖ”
	// 
	//==============================================================
	Skill(){}
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
	// \‘¢‘ÌA—ñ‹“Œ^
	// 
	//==============================================================


	//==============================================================
	// 
	// •Ï”
	// 
	//==============================================================
	float life_time = 0;
	bool skill_end_flag = false;
	bool display_imgui;
	//==============================================================
	// 
	// ’è”
	// 
	//==============================================================
public:
	const float COOL_TIME = 3.0f;
};