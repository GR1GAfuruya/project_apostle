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
	MagicBulletLauncher(Graphics& graphics);
	~MagicBulletLauncher() override {};
	//更新
	void update(Graphics& graphics, float elapsed_time) override;
	//描画
	void render(Graphics& graphics) override;
	//発動
	bool chant(Graphics& graphics, DirectX::XMFLOAT3* init_pos, DirectX::XMFLOAT3 dir);
	//スキルの当たり判定
	void skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func);
	//DebugGUI表示
	void debug_gui() override;

private:
	//スキルの初期パラメーター
	MagicBullet::InitializeParam skill_init_param;
	std::unique_ptr<MeshEffect> test_slash_hit;

};