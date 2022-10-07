#pragma once
#include "skill_launcher.h"

class RestraintLauncher : public SkillLauncher
{
public:
	RestraintLauncher(Graphics& graphics);
	~RestraintLauncher() override {};

	void chant(Graphics& graphics, DirectX::XMFLOAT3* target_pos, DirectX::XMFLOAT3* down_speed);

	//DebugGUI•\Ž¦
	void debug_gui() override;
	

};