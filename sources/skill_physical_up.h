#pragma once
#include "skill.h"
class PhysicalUp : public Skill
{
public:
	//==============================================================
	// 
	// public関数
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