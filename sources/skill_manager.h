#pragma once
#include <array>
#include "sprite_batch.h"
#include "skill_ui.h"
#include "physical_up_launcher.h"
#include "regenerate_launcher.h"
#include "restraint_launcher.h"
#include "skill_spears_sea.h"
#include "magic_bullet_launcher.h"
#include "spears_sea_launcher.h"
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
		PHYSICAL_UP,
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

	//�X�L���r��
	bool chant_physical_up(Graphics& graphics, DirectX::XMFLOAT3* launch_pos, float* add_run_speed, float* add_jump_speed);

	bool chant_regenerate(Graphics& graphics, DirectX::XMFLOAT3* launch_pos, int* health, const int max_health);

	bool chant_restraint(Graphics& graphics, DirectX::XMFLOAT3* target_pos, float* down_speed);

	//�U���X�L���r��
	bool chant_magic_bullet(Graphics& graphics, DirectX::XMFLOAT3* launch_pos, DirectX::XMFLOAT3 dir);
	
	bool chant_spear_sea(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos);

	//�X�L���N�[���^�C���Z�k
	void cool_time_reduction();

	//----�X�L�������蔻��----//
	//MagickBullet
	void judge_magic_bullet_vs_enemy(Capsule object_colider, AddDamageFunc damaged_func);
	//SpearSea
	void judge_spear_sea_vs_enemy(Capsule object_colider, AddDamageFunc damaged_func);
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
	std::shared_ptr<PhysicalUpLauncher> physical_up;
	std::shared_ptr<RegenerateLauncher> regenerate;
	std::shared_ptr<RestraintLauncher> restraint;
	 
	std::shared_ptr<MagicBulletLauncher> magick_bullet;
	std::shared_ptr<SpearSeaLauncher> spear_sea;

	std::array< std::shared_ptr<SkillLauncher>, static_cast<int>(SupportSkillType::SUP_SKILL_MAX)> sup_skill_list;
	std::array< std::shared_ptr<SkillLauncher>, static_cast<int>(AttackSkillType::ATK_SKILL_MAX)> atk_skill_list;

	//�������̃X�L��
	SkillLauncher* selected_sup_skill;
	SkillLauncher* selected_atk_skill;
	//�������̃X�L���̎��
	SupportSkillType selected_sup_skill_type;
	AttackSkillType selected_atk_skill_type;
	//�X�L����UI
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