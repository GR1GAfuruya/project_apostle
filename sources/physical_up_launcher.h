#pragma once
#include "skill_launcher.h"
//==============================================================
// 
// PhysicalUpLauncherクラス　
// 
//==============================================================
class PhysicalUpLauncher : public SkillLauncher
{
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	PhysicalUpLauncher(Graphics& graphics);
	~PhysicalUpLauncher() override {};
	//発動
	bool chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, float* add_run_speed, float* add_jump_speed);

	//DebugGUI表示
	void debug_gui() override;


};