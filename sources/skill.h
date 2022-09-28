#pragma once
#include "graphics.h"
#include "imgui_include.h"
#include "user.h"
class Skill
{
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	
	Skill() {};
	virtual ~Skill() {};

	virtual void initialize(Graphics& graphics) = 0;
	virtual void update(Graphics& graphics,float elapsed_time) = 0;
	virtual void render(Graphics& graphics) = 0;

	bool is_skill_end() { return skill_end_flag; };

	virtual void debug_gui(string str_id) = 0;

	float get_cool_time() { return cool_time; }
 protected:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================


	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================
	float life_time = 0;
	bool skill_end_flag = false;
	float cool_time = 3.0f;
	//==============================================================
	// 
	// �萔
	// 
	//==============================================================
public:
};