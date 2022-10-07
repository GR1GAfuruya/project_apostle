#pragma once
#include "skill.h"
class PhysicalUp : public Skill
{
public:
	//==============================================================
	// 
	// publicä÷êî
	// 
	//==============================================================
	PhysicalUp(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, float* add_run_speed, float* add_jump_speed) {}
	~PhysicalUp() {}

	void initialize(Graphics& graphics) override;
	void update(Graphics& graphics, float elapsed_time)override;
	void render(Graphics& graphics)override;

	void debug_gui(string str_id);
protected:
	//==============================================================
	// 
	// ç\ë¢ëÃÅAóÒãìå^
	// 
	//==============================================================


	//==============================================================
	// 
	// ïœêî
	// 
	//==============================================================

	//==============================================================
	// 
	// íËêî
	// 
	//==============================================================
public:

};