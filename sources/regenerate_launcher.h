#pragma once
#include "skill_launcher.h"

class RegenerateLauncher : public SkillLauncher
{
public:
	RegenerateLauncher(Graphics& graphics);
	~RegenerateLauncher() override {};

	void chant(Graphics& graphics, int* health_up);

	//DebugGUI�\��
	void debug_gui() override;


};