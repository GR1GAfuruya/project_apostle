#pragma once
#include "skill_slot.h"
class SkillManager
{
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	SkillManager();
	~SkillManager();

	void initialize(Graphics& graphics);
	void update(Graphics& graphics, float elapsed_time);
	void render(Graphics& graphics);

	//�T�|�[�g�X�L���r��
	void chant_support_skill(Graphics& graphics);
	//�U���X�L���r��
	void chant_attack_skill(Graphics& graphics);
	//�T�|�[�g�X�L���Z�b�g
	void set_support_skill();
	//�U���X�L���Z�b�g
	void set_attack_skill();

	//�f�o�b�OGUI
	void debug_gui();
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================


	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================
	//�e��X�L��
	std::shared_ptr<SkillSlot> regeneration_skill;

	//�v���C���[�����ۂɎ��s����X�L���̔�
	SkillSlot* support_skill;
	SkillSlot* attack_skill;
	//==============================================================
	// 
	// �萔
	// 
	//==============================================================


};