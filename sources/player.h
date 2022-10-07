#pragma once
#include "device.h"
#include "camera.h"
#include "charactor.h"
#include "skeletal_mesh.h"
#include "gpu_particle.h"
#include "slash.h"
#include "tornado.h"
#include "sword.h"
#include "skill_manager.h"
#include "primitive.h"
//�v���C���[ :final ���̃N���X�̌p�����ł��Ȃ����Ƃ𖾎�����
class Player final :public Charactor
{
public:
	Player(Graphics& graphics, Camera* camera);
	~Player()override;

	//����������
	void initialize();
	//�X�V����
	void update(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
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
	void calc_attack_vs_enemy(Capsule collider, AddDamageFunc damaged_func);

	//�X�L���ƂƓ����蔻��
	void judge_skill_collision(Capsule object_colider, AddDamageFunc damaged_func);
private:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================
	//�A�j���[�V����
	enum  PlayerAnimation
	{
		PLAYER_IDLE,
		PLAYER_WALK,
		PLAYER_ROLL,
		PLAYER_ATK_AIR,
		PLAYER_SUPPORT_MAGIC,
		PLAYER_ATK_LOW,
		PLAYER_JUMP,
		PLAYER_ATK_COMBO1,
		PLAYER_ATK_COMBO2,
		PLAYER_ATK_COMBO3,
		PLAYER_ATK_PULL,
		PLAYER_ATK_BULLET,
		PLAYER_DAMAGE_FRONT,
	};

	enum class State
	{
		IDLE,
		MOVE,
		ROLL,
		JUMP,
		FALL,
		LANDING,
		FRONT_DAMAGE,

	};



	struct AttackParam
	{
		Capsule collision;
		bool is_attack;//�U�������ǂ���
		float power;//�U����
		float invinsible_time;//�U���Ώۂɉۂ����G����
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



private:
	//==============================================================
	// 
	// private�֐�
	// 
	//==============================================================

	//�J��
	void transition_idle_state();
	void transition_attack_combo1_state();
	void transition_attack_combo2_state();
	void transition_attack_combo3_state();
	void transition_attack_combo4_state();
	void transition_attack_pull_state();
	void transition_attack_bullet_state();
	void transition_move_state();
	void transition_jump_state();
	void transition_avoidance_state();
	void transition_support_magic_state();
	void transition_damage_front_state();


	//////�A�j���[�V�����A�b�v�f�[�g
	void update_idle_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_combo1_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_combo2_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_combo3_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_combo4_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_pull_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_bullet_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_move_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_jump_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_avoidance_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_support_magic_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_damage_front_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);

	typedef void (Player::* ActUpdate)(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	//
	void Attack(Graphics& graphics, float elapsed_time);

	void select_support_skill();

	//�v���C���[�̈ړ����͏���
	bool input_move(float elapsedTime, Camera* camera);
	const DirectX::XMFLOAT3 get_move_vec(Camera* camera) const;
	//�W�����v���͏���
	void input_jump();
	//������
	void input_avoidance();
	//�T�|�[�g�X�L���r������
	void input_chant_support_skill(Graphics& graphics);
	//�U���X�L���r������
	void input_chant_attack_skill(Graphics& graphics);
	
	//���n������
	void on_landing()override;

	void on_dead() override;

	void on_damaged() override;

	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================
	ActUpdate p_update = &Player::update_idle_state;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;

	State state;

	GamePad* game_pad;
	Mouse* mouse;

	// �X�P���^�����b�V���̎���
	std::unique_ptr <SkeletalMesh> model;
	float move_speed = 30.0f;
	float turn_speed = DirectX::XMConvertToRadians(720);

	//�W�����v�X�s�[�h
	float jump_speed = 35.0f;
	//������W�����v���Ă邩
	int jump_count = 0;
	//�W�����v�\��
	int jump_limit = 1;
	float avoidance_speed = 50;
	bool display_player_imgui = true;
	/*-------�U���֘A--------------------------*/
	//�U������
	float attack_time;
	//�U����
	int add_damage;

	std::unique_ptr<GPU_Particles> attack1;
	std::unique_ptr <SkillManager> skill_manager;
	std::unique_ptr<Slash> slash_efect;
	std::unique_ptr<Sword> sword;
	skeleton::bone sword_hand;
	skeleton::bone right_hand;

	//�����蔻��p�ϐ�
	DirectX::XMFLOAT3 radius_aabb = { 5, 5, 5 };
	AttackParam attack_sword_param;
public:
	//�_���[�W���󂯂��Ƃ��ɌĂ΂�� *�֐����ĂԂ̂̓_���[�W��^�����I�u�W�F�N�g
	AddDamageFunc damaged_function;
	Capsule collider;
};
