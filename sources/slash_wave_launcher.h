#pragma once
#include "skill_launcher.h"
#include "skill_slash_wave.h"
class SlashWaveLauncher : public SkillLauncher
{
public:
	SlashWaveLauncher();
	~SlashWaveLauncher() override {};

	bool chant(DirectX::XMFLOAT3* launch_pos, DirectX::XMFLOAT3* dir);

	void skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera);
	//DebugGUI•\Ž¦
	void debug_gui() override;
	AttackParam atk_param;

	SlashWave::PublicParam init_param;
};
