#pragma once
#include "skill_launcher.h"
#include "skill_magic_bullet.h"
//==============================================================
// 
// MagicBulletLauncher�N���X�@
// 
//==============================================================
class MagicBulletLauncher : public SkillLauncher
{
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	MagicBulletLauncher(Graphics& graphics);
	~MagicBulletLauncher() override {};
	//�X�V
	void update(Graphics& graphics, float elapsed_time) override;
	//�`��
	void render(Graphics& graphics) override;
	//����
	bool chant(Graphics& graphics, DirectX::XMFLOAT3* init_pos, DirectX::XMFLOAT3 dir);
	//�X�L���̓����蔻��
	void skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func);
	//DebugGUI�\��
	void debug_gui() override;

private:
	//�X�L���̏����p�����[�^�[
	MagicBullet::InitializeParam skill_init_param;
	std::unique_ptr<MeshEffect> test_slash_hit;

};