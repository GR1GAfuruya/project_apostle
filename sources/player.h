#pragma once
#include "device.h"
#include "camera.h"
#include "charactor.h"
#include "skeletal_mesh.h"
#include "gpu_particle.h"
#include "mesh_effect.h"
#include "sword.h"
#include "skill_manager.h"
#include "primitive.h"
#include "player_ui.h"

//�v���C���[ :final ���̃N���X�̌p�����ł��Ȃ����Ƃ𖾎�����
class Player final :public Charactor
{
public:
	Player(Graphics& graphics, Camera* camera);
	~Player()override;

	//����������
	void initialize();
	//�X�V����
	void update(Graphics& graphics, float elapsed_time, Camera* camera);
	//�`�揈��
	//�f�B�t�@�[�h�Ń����_�����O����I�u�W�F�N�g
	void render_d(Graphics& graphics, float elapsed_time, Camera* camera);
	//�t�H���[�h�����_�����O����I�u�W�F�N�g
	void render_f(Graphics& graphics, float elapsed_time, Camera* camera);

	void render_ui(Graphics& graphics, float elapsed_time);
	//�f�o�b�O�pGUI�`��
	void debug_gui(Graphics& graphics);
	//�v���C���[�̍�������̈ʒu
	DirectX::XMFLOAT3 get_waist_position() { return DirectX::XMFLOAT3(position.x, position.y + height / 2, position.z); }
	//�J�������v���C���[������Ƃ��ɒ�������|�C���g
	DirectX::XMFLOAT3 get_gazing_point() { return DirectX::XMFLOAT3(position.x, position.y + (height + 3), position.z); }

	//�v���C���[�̃R���W�����ƓG�̓����蔻��
	void calc_collision_vs_enemy(Capsule capsule_collider, float colider_height);

	//�v���C���[�̍U���ƓG�̓����蔻��
	void calc_attack_vs_enemy(Capsule collider, AddDamageFunc damaged_func, Camera* camera);

	//�X�L���ƓG�̓����蔻��
	void judge_skill_collision(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera);
private:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================
	//�A�j���[�V����
	enum  PlayerAnimation
	{
		PLAYER_IDLE,//�ҋ@
		PLAYER_RUN,//����
		PLAYER_ROLL,//���
		PLAYER_JUMP,//�W�����v
		PLAYER_DAMAGE_FRONT,//�O�����_��
		PLAYER_ATK_SPRING_SLASH,//�O��]�؂�
		PLAYER_PULL_SLASH,//�G�������t���Ďa��
		PLAYER_ATK_GROUND,//�n�ʂɎ��t���Č��񂹂݂�����
		PLAYER_MAGIC_BUFF,//�o�t
		PLAYER_MAGIC_SLASH_UP,//�󒆂Ɋ����グ�a��
		PLAYER_MAGIC_BULLET,//���������@�e�ł悤��
		PLAYER_ATK_FORWARD_SLASH,//�O�i�a��
		PLAYER_ATK_AIR,//�W�����v���Ēn�ʂɖ��@����
		PLAYER_ATK_COMBO1,//�R���{2-1
		PLAYER_ATK_COMBO2,//�R���{2-2
		PLAYER_ATK_COMBO3,//�R���{2-3
		PLAYER_ATK_DODGE_BACK,//����ɉ�����Ȃ��疂�@
	};

	enum class State
	{
		IDLE,
		MOVE,
		ROLL,
		JUMP,
		LANDING,
		FRONT_DAMAGE,
		NORMAL_ATTACK,
		SKILL,

	};

	//--------------------------------------------------------------
	//  �萔
	//--------------------------------------------------------------
	
	 //�U��1���ڂ̗P�\����
	static constexpr float ATTACK_TYPE1_MAX_TIME = 0.3f;
	//�U��2���ڂ̗P�\����
	static constexpr float ATTACK_TYPE2_MAX_TIME = 0.2f;
	//�U��3���ڂ̗P�\����
	static constexpr float ATTACK_TYPE3_MAX_TIME = 0.2f;

	//�U��1���ڂ̍U����
	static constexpr int ATTACK_TYPE1_POWER = 1;
	//�U��2���ڂ̍U����
	static constexpr int ATTACK_TYPE2_POWER = 2;
	//�U��3���ڂ̍U����
	static constexpr int ATTACK_TYPE3_POWER = 4;


private:
	//==============================================================
	// 
	// private�֐�
	// 
	//==============================================================

	//**********�J��*************//
	
	void transition_idle_state();//�ҋ@
	void transition_move_state();//����
	void transition_avoidance_state();//���
	void transition_jump_state();//�W�����v
	void transition_damage_front_state();//�O�����_��
	void transition_r_attack_spring_slash_state();//�O��]�؂�
	void transition_attack_pull_slash_state();//�G�������t���Ďa��
	void transition_attack_ground_state();//�n�ʂɎ��t���Č��񂹂݂�����
	void transition_magic_buff_state();//�o�t
	void transition_attack_bullet_state();//���������@�e�ł悤��
	void transition_attack_slash_up_state();//�󒆂Ɋ����グ�a��
	void transition_r_attack_forward_state();//�O�i�a��
	void transition_attack_air_state();//�W�����v���Ēn�ʂɖ��@����
	void transition_r_attack_combo1_state();//�R���{2-1
	void transition_r_attack_combo2_state();//�R���{2-2
	void transition_r_attack_combo3_state();//�R���{2-3
	void transition_r_attack_dodge_back_state();//����ɉ�����Ȃ��疂�@


	//********�e�X�e�[�g�̃A�b�v�f�[�g**********//r_�̓��[�g���[�V�����t��
	void update_idle_state(Graphics& graphics, float elapsed_time, Camera* camera);
	void update_move_state(Graphics& graphics, float elapsed_time, Camera* camera);//����
	void update_avoidance_state(Graphics& graphics, float elapsed_time, Camera* camera);//���
	void update_jump_state(Graphics& graphics, float elapsed_time, Camera* camera);//�W�����v
	void update_damage_front_state(Graphics& graphics, float elapsed_time, Camera* camera);//�O�����_��
	void update_r_attack_spring_slash_state(Graphics& graphics, float elapsed_time, Camera* camera);//�O��]�؂�
	void update_attack_pull_slash_state(Graphics& graphics, float elapsed_time, Camera* camera);//�G�������t���Ďa��
	void update_attack_ground_state(Graphics& graphics, float elapsed_time, Camera* camera);//�n�ʂɎ��t���Č��񂹂݂�����
	void update_magic_buff_state(Graphics& graphics, float elapsed_time, Camera* camera);//�o�t
	void update_attack_bullet_state(Graphics& graphics, float elapsed_time, Camera* camera);//�󒆂Ɋ����グ�a��
	void update_attack_slash_up_state(Graphics& graphics, float elapsed_time, Camera* camera);//���������@�e�ł悤��
	void update_r_attack_forward_state(Graphics& graphics, float elapsed_time, Camera* camera);//�O�i�a��
	void update_attack_air_state(Graphics& graphics, float elapsed_time, Camera* camera);//�W�����v���Ēn�ʂɖ��@����
	void update_r_attack_combo1_state(Graphics& graphics, float elapsed_time, Camera* camera);//�R���{2-1
	void update_r_attack_combo2_state(Graphics& graphics, float elapsed_time, Camera* camera);//�R���{2-2
	void update_r_attack_combo3_state(Graphics& graphics, float elapsed_time, Camera* camera);//�R���{2-3
	void update_r_attack_dodge_back_state(Graphics& graphics, float elapsed_time, Camera* camera);//����ɉ�����Ȃ��疂�@

	typedef void (Player::* ActUpdate)(Graphics& graphics, float elapsed_time, Camera* camera);
	//
	void attack_combo4_effect(Graphics& graphics, float elapsed_time);

	//�v���C���[�̈ړ����͏���
	bool input_move(float elapsedTime, Camera* camera);

	//�����t���̈ړ��i�U�����Ȃǂ̈ړ����́j
	bool input_move(float elapsedTime, Camera* camera,float restriction_move, float restriction_turn);
	const DirectX::XMFLOAT3 get_move_vec(Camera* camera) const;
	//�W�����v���͏���
	void input_jump();
	//������
	void input_avoidance();
	//�T�|�[�g�X�L���r������
	void input_chant_support_skill(Graphics& graphics, Camera* camera);
	//�U���X�L���r������
	void input_chant_attack_skill(Graphics& graphics, Camera* camera);
	
	//���n������
	void on_landing()override;
	//���S�����Ƃ��̏���
	void on_dead() override;
	//�_���[�W���󂯂����̏���
	void on_damaged(WINCE_TYPE type) override;
	//���[�g���[�V����
	void root_motion(DirectX::XMFLOAT3 dir, float speed);
	void root_motion_manual(DirectX::XMFLOAT3 dir, float speed);
	//�������V����
	bool floating();
	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================
	ActUpdate p_update = &Player::update_idle_state;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> attack4_emit_cs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> attack4_update_cs;

	State state;

	GamePad* game_pad;
	Mouse* mouse;

	// �X�P���^�����b�V���̎���
	std::unique_ptr <SkeletalMesh> model;
	float move_speed = 30.0f;
	float skill_add_move_speed = 0.0f;
	float turn_speed = DirectX::XMConvertToRadians(720);

	//�W�����v�X�s�[�h
	float jump_speed = 35.0f;
	float skill_add_jump_speed = 0;
	//������W�����v���Ă邩
	int jump_count = 0;
	//�W�����v�\��
	int jump_limit = 1;
	float avoidance_speed = 50;
	bool display_player_imgui = false;

	DirectX::XMFLOAT3 left_hand_pos;
	/*-------�U���֘A--------------------------*/
	//�U������
	float attack_time;
	//�U����
	int add_damage;
	//���V�x
	float floating_value = 10.0f;
	float sword_swing_speed = 1500.0f;
	//std::unique_ptr<GPU_Particles> attack1;
	std::unique_ptr <SkillManager> skill_manager;
	std::unique_ptr<MeshEffect> slash_efects[3];
	std::unique_ptr<MeshEffect> test_slash_hit;
	std::unique_ptr<Sword> sword;
	std::unique_ptr<PlayerUI> ui;

	//std::unique_ptr<EffekseerEffect> test_slash_hit;

	skeleton::bone left_hand;
	skeleton::bone right_hand;
	DirectX::XMFLOAT3 root_defference_velocity;
	//�����蔻��p�ϐ�
	DirectX::XMFLOAT3 radius_aabb = { 5, 5, 5 };
	AttackParam attack_sword_param;
	//���[�g���[�V����
	skeleton::bone root;
	DirectX::XMFLOAT3 root_motion_pos = {0,0,0};
	float add_root_speed = 1.1f;
	bool is_root_motion = false;
public:
	//�_���[�W���󂯂��Ƃ��ɌĂ΂�� *�֐����ĂԂ̂̓_���[�W��^�����I�u�W�F�N�g
	AddDamageFunc damaged_function;
	Capsule collider;
};
