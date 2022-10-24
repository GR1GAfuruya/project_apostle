#pragma once
#include "skill_launcher.h"

class SpearSeaLauncher : public SkillLauncher
{
public:
	SpearSeaLauncher(Graphics& graphics);
	~SpearSeaLauncher() override {};

	bool chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos);

	void skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func);
	//DebugGUI�\��
	void debug_gui() override;


};