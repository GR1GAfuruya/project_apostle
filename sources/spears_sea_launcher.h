#pragma once
#include "skill_launcher.h"

class SpearSeaLauncher : public SkillLauncher
{
public:
	SpearSeaLauncher(Graphics& graphics);
	~SpearSeaLauncher() override {};

	void chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos);

	void skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func);
	//DebugGUI•\Ž¦
	void debug_gui() override;


};