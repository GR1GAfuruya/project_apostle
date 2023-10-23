#pragma once
#include "skill_launcher.h"
#include "skill_spears_sea.h"
class SpearSeaLauncher : public SkillLauncher
{
public:
	SpearSeaLauncher();
	~SpearSeaLauncher() override {};

	bool chant(DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos);

	void skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera);
	//DebugGUI•\Ž¦
	void debug_gui() override;
	AttackParam atk_param;

	SpearsSea::PublicParam init_param;
};