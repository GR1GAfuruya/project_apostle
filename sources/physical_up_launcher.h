#pragma once
#include "skill_launcher.h"

class PhysicalUpLauncher : public SkillLauncher
{
public:
	PhysicalUpLauncher(Graphics& graphics);
	~PhysicalUpLauncher() override {};

	bool chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, float* add_run_speed, float* add_jump_speed);

	//DebugGUI•\Ž¦
	void debug_gui() override;


};