#pragma once
#include "charactor.h"
#include "skeletal_mesh.h"
#include "gpu_particle.h"

#include "boss_charge_attack.h"
#include "light.h"
#include "primitive.h"
#include "boss_ui.h"
class Boss :public Charactor
{
private:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================

	enum  BossAnimation
	{
		AIRBORNE,
		ATTACK,
		DAMAGE,
		DEAD,
		DIE,
		DOWN,
		DOWNDEAD,
		FALL,
		GROGGY_END,
		GROGGY_LOOP,
		GROGGY_START,
		IDLE,
		RUN,
		SKILL_1,
		SKILL_2_END,
		SKILL_2_LOOP,
		SKILL_2_START,
		SKILL_3,
		STAND,
		STUN,
		WALK

	};
	enum class State
	{
		AIRBORNE,
		ATTACK,
		DAMAGE,
		DEAD,
		DIE,
		DOWN,
		DOWNDEAD,
		FALL,
		GROGGY_END,
		GROGGY_LOOP,
		GROGGY_START,
		IDLE,
		RUN,
		SKILL_1,
		SKILL_2_END,
		SKILL_2_LOOP,
		SKILL_2_START,
		SKILL_3,
		STAND,
		STUN,
		WALK

	};

	enum class ATTACK_TYPE
	{
		NORMAL,
		SKILL1,
		SKILL2,
		SKILL3,
		MAX_NUM
	};
	struct BodyCollision
	{
		Capsule capsule;
		float height;
	};


public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================

	Boss(Graphics& graphics);
	~Boss() {};
	
	//������
	void initialize();

	//�X�V
	void update(Graphics& graphics, float elapsed_time, Camera* camera);

	//�`�揈��
	//�f�B�t�@�[�h�Ń����_�����O����I�u�W�F�N�g
	void render_d(Graphics& graphics, float elapsed_time);
	//�t�H���[�h�����_�����O����I�u�W�F�N�g
	void render_f(Graphics& graphics, float elapsed_time);
	void render_ui(Graphics& graphics, float elapsed_time);

	//�f�o�b�O�pGUI�`��
	void debug_gui();

	//�v���C���[�̍U���Ƃ̓����蔻��
	void calc_attack_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func);

	//�U���Ώۂ̈ʒu���擾
	void set_location_of_attack_target(DirectX::XMFLOAT3 target) { target_pos = target; }

	BodyCollision get_body_collision() { return boss_body_collision; }

	//�J�������v���C���[������Ƃ��ɒ�������|�C���g
	DirectX::XMFLOAT3 get_gazing_point() { return DirectX::XMFLOAT3(position.x, position.y + (height + 3), position.z); }
	//==============================================================
	// 
	// private�֐�
	// 
	//==============================================================

	/*--------------------��ԑJ��------------------------*/

	//---------------------------//
	//			�ړ��n			 //
	//---------------------------//

	//�ҋ@��ԂɈڍs
	void transition_idle_state();
	//������ԂɈڍs
	void transition_walk_state();
	//�����ԂɈڍs
	void transition_run_state();

	//---------------------------//
	//			�U���n			 //
	//---------------------------//
	
	//�E�r���U��U����ԂɈڍs
	void transition_attack_state();
	//���U��グ�n�ʂɑł��t����U����ԂɈڍs
	void transition_skill_1_state();
	//�`���[�W�I����ԂɈڍs
	void transition_skill_2_end_state();
	//�`���[�W����ԂɈڍs
	void transition_skill_2_loop_state();
	//�`���[�W�J�n��ԂɈڍs
	void transition_skill_2_start_state();
	//�������߉E�r��U�蕥���U����ԂɈڍs
	void transition_skill_3_state();

	//---------------------------//
	//			�_�E���n		 //
	//---------------------------//
	
	//�󒆐�����я�ԂɈڍs
	void transition_air_bone_state();
	//�_���[�W���ݏ�ԂɈڍs
	void transition_damage_state();
	//���S���i�s���j��ԂɈڍs
	void transition_dead_state();
	//���S��ԂɈڍs
	void transition_die_state();
	//�|��Ȃ���ċz��ԂɈڍs
	void transition_down_state();
	//�_�E����ԁi�s���j�Ɉڍs
	void transition_downdead_state();
	//�󒆂ɐ�����΂��ꂽ��Ԃ��痎����ԂɈڍs
	void transition_fall_state();
	//�n�ʂɎ�����_�E���i�����j��ԂɈڍs
	void transitiond_groggy_start_state();
	//�n�ʂɎ�����_�E���i���[�v�j��ԂɈڍs
	void transition_groggy_loop_state();
	//�n�ʂɎ�����_�E������N���オ���ԂɈڍs
	void transition_groggy_end_state();
	//�|�ꂽ��Ԃ���N���オ���ԂɈڍs
	void transition_stand_state();
	//�������܂܃X�^����ԂɈڍs
	void transition_stun_state();


	/*---------------��ԍX�V------------------------*/

	//---------------------------//
	//			�ړ��n			 //
	//---------------------------//
	
	//�ҋ@��ԍX�V
	void update_idle_state(Graphics& graphics, float elapsed_time);
	//������ԍX�V
	void update_walk_state(Graphics& graphics, float elapsed_time);
	//�����ԍX�V
	void update_run_state(Graphics& graphics, float elapsed_time);


	//---------------------------//
	//			�U���n			 //
	//---------------------------//
	
	//�E�r���U��U����ԍX�V
	void update_attack_state(Graphics& graphics, float elapsed_time);
	//���U��グ�n�ʂɑł��t����U����ԍX�V
	void update_skill_1_state(Graphics& graphics, float elapsed_time);
	//�`���[�W�I����ԍX�V
	void update_skill_2_end_state(Graphics& graphics, float elapsed_time);
	//�`���[�W����ԍX�V
	void update_skill_2_loop_state(Graphics& graphics, float elapsed_time);
	//�`���[�W�J�n��ԍX�V
	void update_skill_2_start_state(Graphics& graphics, float elapsed_time);
	//�������߉E�r��U�蕥���U����ԍX�V
	void update_skill_3_state(Graphics& graphics, float elapsed_time);

	//�U�����@�I��
	void select_attack_type();

	//---------------------------//
	//			�_�E���n		 //
	//---------------------------//
	//�󒆐�����я�ԍX�V
	void update_air_bone_state(Graphics& graphics, float elapsed_time);
	//�_���[�W���ݏ�ԍX�V
	void update_damage_state(Graphics& graphics, float elapsed_time);
	//���S���i�s���j��ԍX�V
	void update_dead_state(Graphics& graphics, float elapsed_time);
	//���S��ԍX�V
	void update_die_state(Graphics& graphics, float elapsed_time);
	//�|��Ȃ���ċz��ԍX�V
	void update_down_state(Graphics& graphics, float elapsed_time);
	//�_�E����ԁi�s���j��ԍX�V
	void update_downdead_state(Graphics& graphics, float elapsed_time);
	//�󒆂ɐ�����΂��ꂽ��Ԃ��痎����ԍX�V
	void update_fall_state(Graphics& graphics, float elapsed_time);
	//�n�ʂɎ�����_�E���i�����j��ԍX�V
	void update_groggy_start_state(Graphics& graphics, float elapsed_time);
	//�n�ʂɎ�����_�E���i���[�v�j��ԍX�V
	void update_groggy_loop_state(Graphics& graphics, float elapsed_time);
	//�n�ʂɎ�����_�E������N���オ���ԍX�V
	void update_groggy_end_state(Graphics& graphics, float elapsed_time);
	//�|�ꂽ��Ԃ���N���オ���ԍX�V
	void update_stdand_state(Graphics& graphics, float elapsed_time);
	//�������܂܃X�^����ԍX�V
	void update_stun_state(Graphics& graphics, float elapsed_time);

	//�[�|�|�|
	void on_dead() override;
	void on_damaged(WINCE_TYPE type) override;

	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================
	typedef void (Boss::* ActUpdate)(Graphics& graphics, float elapsed_time);
	ActUpdate act_update = &Boss::update_idle_state;
	std::unique_ptr<SkeletalMesh> model;
	std::unique_ptr<ChargeAttack> efc_charge_attack;
	std::unique_ptr<BossUi> ui;

	//���r�̃{�[��
	skeleton::bone sickle_hand;

	float action_time = 0;
	float move_speed = 30.0f;
	float turn_speed = 5.0f;
	bool display_imgui = false;

	//�X�e�[�g�̃^�C�}�[
	float state_timer;

	float state_duration;
	//�U���Ώ�
	DirectX::XMFLOAT3 target_pos;

	//
	State state;
	
	AttackParam sickle_attack_param;
	BodyCollision boss_body_collision;

#if _DEBUG
	bool is_update = true;
	bool is_render = true;
#endif
	//==============================================================
	// 
	// �萔
	// 
	//==============================================================
	//�����X�s�[�h
	float WALK_SPEED = 15;
	//����X�s�[�h
	float RUN_SPEED = 30;
	//�ʏ�U���̃N�[���^�C��
	float ATTACK_ACTION_LENGTH = 17;
	//�ʏ�U���̃N�[���^�C��
	float NORMAL_ATTACK_COOLTIME = 1;
	//
	float SKILL1_COOLTIME = 2;
	//
	float SKILL2_COOLTIME = 3;
	//
	float SKILL3_COOLTIME = 2;
	//�_���[�W���󂯂��Ƃ��̃X�^������
	float DAMAGE_STUN_DURATION = 0.7f;
public:

	AddDamageFunc damaged_function;
};