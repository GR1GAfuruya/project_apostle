#pragma once
#include "skill_launcher.h"

class RegenerateLauncher : public SkillLauncher
{
public:
	RegenerateLauncher(Graphics& graphics);
	~RegenerateLauncher() override {};

	bool chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, int* health);

	//DebugGUI•\Ž¦
	void debug_gui() override;


};