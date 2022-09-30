#pragma once
#include "skill_slot.h"
#include <array>
#include "sprite_batch.h"
class SkillManager
{
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	SkillManager(Graphics& graphics);
	~SkillManager() {};

	//������
	void initialize(Graphics& graphics);
	//�X�V
	void update(Graphics& graphics, float elapsed_time);
	//�`��
	void render(Graphics& graphics);


	void ui_render(Graphics& graphics, float elapsed_time);

	//�T�|�[�g�X�L���r��
	void chant_support_skill(Graphics& graphics);
	//�U���X�L���r��
	void chant_attack_skill(Graphics& graphics);
	//�T�|�[�g�X�L���Z�b�g
	void set_support_skill();
	//�U���X�L���Z�b�g
	void set_attack_skill();

	//�f�o�b�OGUI
	void debug_gui(Graphics& graphics);
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
	std::shared_ptr<SkillSlot> physical_up_skill;
	std::shared_ptr<SkillSlot> regeneration_skill;
	std::shared_ptr<SkillSlot> restraint_skill;
	std::shared_ptr<SkillSlot> magic_bullet_skill;
	std::shared_ptr<SkillSlot> spears_sea_skill;
	//std::shared_ptr<SkillSlot> regeneration_skill;
	std::array<std::shared_ptr<SkillSlot>, 3> support_skill_slots;
	std::array<std::shared_ptr<SkillSlot>, 2> attack_skill_slots;
	//�v���C���[�����ۂɎ��s����X�L���̔�
	SkillSlot* support_skill;
	SkillSlot* attack_skill;

	std::unique_ptr<SpriteBatch> skill_slot_icon;
	bool display_imgui;
	//==============================================================
	// 
	// �萔
	// 
	//==============================================================


};