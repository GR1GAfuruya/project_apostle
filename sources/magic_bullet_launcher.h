#pragma once
#include "skill_launcher.h"
#include "skill_magic_bullet.h"
//==============================================================
// 
// MagicBulletLauncherクラス　
// 
//==============================================================
class MagicBulletLauncher : public SkillLauncher
{
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	MagicBulletLauncher();
	~MagicBulletLauncher() override {};
	//更新
	void update(float elapsed_time) override;
	//描画
	void render(Camera* camera) override;
	//発動
	bool chant(DirectX::XMFLOAT3* init_pos, DirectX::XMFLOAT3* dir);
	//スキルの当たり判定
	void skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera);
	//DebugGUI表示
	void debug_gui() override;

private:
	AttackParam atk_param;
	//スキルの初期パラメーター

	MagicBullet::PublicParam skill_init_param;
	std::unique_ptr<MeshEffect> test_slash_hit;

};