#pragma once
#include "skill_launcher.h"
#include "skill_lightning_rain.h"
class LightningRainLauncher : public SkillLauncher
{
public:
	LightningRainLauncher(Graphics& graphics);
	~LightningRainLauncher() override {};

	bool chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos);

	void skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera);
	//DebugGUI�\��
	void debug_gui() override;

	LightningRain::PublicParam init_param;
};