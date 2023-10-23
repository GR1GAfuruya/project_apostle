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
	MagicBulletLauncher();
	~MagicBulletLauncher() override {};
	//�X�V
	void update(float elapsed_time) override;
	//�`��
	void render(Camera* camera) override;
	//����
	bool chant(DirectX::XMFLOAT3* init_pos, DirectX::XMFLOAT3* dir);
	//�X�L���̓����蔻��
	void skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera);
	//DebugGUI�\��
	void debug_gui() override;

private:
	AttackParam atk_param;
	//�X�L���̏����p�����[�^�[

	MagicBullet::PublicParam skill_init_param;
	std::unique_ptr<MeshEffect> test_slash_hit;

};