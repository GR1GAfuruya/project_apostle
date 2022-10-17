#pragma once
#include "skill_launcher.h"

class PhycicalUpLauncher : public SkillLauncher
{
public:
	PhycicalUpLauncher(Graphics& graphics);
	~PhycicalUpLauncher() override {};

	bool chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, float* add_run_speed, float* add_jump_speed);

	//DebugGUI�\��
	void debug_gui() override;


};