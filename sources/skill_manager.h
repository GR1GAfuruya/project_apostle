#pragma once
#include "support_skill_slot.h"
#include "attack_skill_slot.h"
#include <array>
#include "sprite_batch.h"
#include "skill_ui.h"
#include "skill_magic_bullet.h"
#include "skill_spears_sea.h"
#define SP_SKILLTYPE SkillManager::SupportSkillType
#define ATK_SKILLTYPE SkillManager::AttackSkillType
class SkillManager
{
public:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================
	enum class SupportSkillType
	{
		PHYCICAL_UP,
		REGENERATE,
		RESTRAINNT,
		SUP_SKILL_MAX
	};
	enum class AttackSkillType
	{
		MAGICBULLET,
		SPEARS_SEA,
		ATK_SKILL_MAX
	};

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

	//UI�`��
	void ui_render(Graphics& graphics, float elapsed_time);

	SupportSkillType get_selected_sup_skill_type() { return selected_sup_skill_type; }

	AttackSkillType get_selected_atk_skill_type() { return selected_atk_skill_type; }

	//�T�|�[�g�X�L���r��
	void chant_phycical_up(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, float* add_run_speed, float* add_jump_speed);

	void chant_regenerate(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, int* health);

	void chant_restraint(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3* target_pos);

	void chant_magic_bullet(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 dir);
	//�U���X�L���r��
	void chant_spear_sea(Graphics& graphics, DirectX::XMFLOAT3 launch_pos);
	
	//�X�L���Z���N�g�����ǂ���
	bool is_selecting_skill() {
		if (is_selecting_support_skill) return true;
		if (is_selecting_attack_skill) return true;
		return false;
	}

	int select_skill_slot(DirectX::XMFLOAT2 stick_vec,int slot_num);
	//�f�o�b�OGUI
	void debug_gui(Graphics& graphics);

private:
	//�T�|�[�g�X�L���Z�b�g
	void set_support_skill(int skill_index);
	//�U���X�L���Z�b�g
	void set_attack_skill(int skill_index);

	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================
	//�v���C���[�����ۂɎ��s����X�L���̔�
	std::unique_ptr<SkillSlot> support_skills;
	std::unique_ptr<SkillSlot> attack_skills;

	SupportSkillType selected_sup_skill_type;
	AttackSkillType selected_atk_skill_type;
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