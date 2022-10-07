#pragma once
#include "skill_launcher.h"
#include "skill_magic_bullet.h"
class MagicBulletLauncher : public SkillLauncher
{
public:
	MagicBulletLauncher(Graphics& graphics);
	~MagicBulletLauncher() override {};

	void chant(Graphics& graphics, DirectX::XMFLOAT3 init_pos, DirectX::XMFLOAT3 dir);

	void skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func);
	//DebugGUI•\Ž¦
	void debug_gui() override;


private:
	MagicBullet::InitializeParam skill_init_param;

};