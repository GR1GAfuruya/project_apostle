#pragma once
#include "support_skill_slot.h"
#include "attack_skill_slot.h"
#include <array>
#include "sprite_batch.h"
#include "skill_ui.h"
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
	void set_support_skill(int skill_index);
	//�U���X�L���Z�b�g
	void set_attack_skill(int skill_index);

	int select_skill_slot(DirectX::XMFLOAT2 stick_vec);
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
	std::shared_ptr<SupportSkillSlot> physical_up_skill;
	std::shared_ptr<SupportSkillSlot> regeneration_skill;
	std::shared_ptr<SupportSkillSlot> restraint_skill;
	std::shared_ptr<AttackSkillSlot> magic_bullet_skill;
	std::shared_ptr<AttackSkillSlot> spears_sea_skill;
	//std::shared_ptr<SkillSlot> regeneration_skill;
	std::array<std::shared_ptr<SupportSkillSlot>, 3> support_skill_slots;
	std::array<std::shared_ptr<AttackSkillSlot>, 2> attack_skill_slots;
	//�v���C���[�����ۂɎ��s����X�L���̔�
	SupportSkillSlot* support_skill;
	AttackSkillSlot* attack_skill;

	
	std::unique_ptr<SkillUI> sup_slots_ui;
	std::unique_ptr<SkillUI> atk_slots_ui;
	bool display_imgui;
	bool is_selecting_support_skill;
	bool is_selecting_attack_skill;
	
	//==============================================================
	// 
	// �萔
	// 
	//==============================================================


};