#pragma once
#include "skill_launcher.h"
#include "skill_slash_wave.h"
class SlashWaveLauncher : public SkillLauncher
{
public:
	SlashWaveLauncher(Graphics& graphics);
	~SlashWaveLauncher() override {};

	bool chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3* dir);

	void skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera);
	//DebugGUI•\Ž¦
	void debug_gui() override;

	SlashWave::PublicParam init_param;
};
