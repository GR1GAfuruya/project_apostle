#pragma once
#include "skill_launcher.h"
//==============================================================
// 
// PhysicalUpLauncher�N���X�@
// 
//==============================================================
class PhysicalUpLauncher : public SkillLauncher
{
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	PhysicalUpLauncher();
	~PhysicalUpLauncher() override {};
	//����
	bool chant(DirectX::XMFLOAT3* launch_pos, float* add_run_speed, float* add_jump_speed);

	//DebugGUI�\��
	void debug_gui() override;


};