#pragma once
#include "device.h"
#include "camera.h"
#include "charactor.h"
#include "skeletal_mesh.h"
#include "gpu_particle.h"
#include "slash.h"
#include "tornado.h"
#include "sword.h"
//#include	"Effect.h"
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
	//�f�o�b�O�pGUI�`��
	void debug_gui();
	//�v���C���[�̍�������̈ʒu
	DirectX::XMFLOAT3 get_waist_position() { return DirectX::XMFLOAT3(position.x, position.y + height, position.z); }

	//�v���C���[�̃R���W�����ƓG�̓����蔻��
	void calc_collision_vs_enemy(DirectX::XMFLOAT3 colider_position,float colider_radius, float colider_height);

	//�v���C���[�̍U���ƓG�̓����蔻��
	void calc_attack_vs_enemy(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func);

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
		PLAYER_ATK_MAGIC,
		PLAYER_ATK_LOW,
		PLAYER_JUMP,
		PLAYER_ATK_COMBO1,
		PLAYER_ATK_COMBO2,
		PLAYER_ATK_COMBO3,
	};

	enum class State
	{
		IDLE,
		MOVE,
		ROLL,
		JUMP,
		FALL,
		LANDING,
	};

	
	struct Capsule
	{
		DirectX::XMFLOAT3 start;
		DirectX::XMFLOAT3 end;
		float radius;
	};

	struct AttackParam
	{
		Capsule collision;
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
	// �v���C�x�[�g�֐�
	// 
	//==============================================================

	//�J��
	void transition_idle_state();
	void transition_attack_combo1_state();
	void transition_attack_combo2_state();
	void transition_attack_combo3_state();
	void transition_attack_combo4_state();
	void transition_move_state();
	void transition_jump_state();
	void transition_avoidance_state();
	//void transition_landing_state();


	//////�A�j���[�V�����A�b�v�f�[�g
	void update_idle_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_combo1_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_combo2_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_combo3_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_combo4_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_move_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_jump_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_avoidance_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	//void update_fall_state(float elapsed_time, Camera* camera);
	//void update_landing_state(float elapsed_time, Camera* camera);

	typedef void (Player::* ActUpdate)(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	//
	void Attack(Graphics& graphics, float elapsed_time);

	//�v���C���[�̈ړ����͏���
	bool input_move(float elapsedTime, Camera* camera);
	const DirectX::XMFLOAT3 get_move_vec(Camera* camera) const;
	//�W�����v���͏���
	void input_jump();
	//������
	void input_avoidance();
	//���n������
	void on_landing()override;

	void on_damaged(int damage, float InvincibleTime);

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
	bool display_player_imgui = false;
	/*-------�U���֘A--------------------------*/
	//�U������
	float attack_time;
	//�U����
	int add_damage;

	std::unique_ptr<GPU_Particles> attack1;
	std::unique_ptr<Slash> slash_efect;
	std::unique_ptr<Sword> sword;
	skeleton::bone sword_hand;
	skeleton::bone sword_bone;

	//�����蔻��p�ϐ�
	DirectX::XMFLOAT3 radius_aabb = { 5, 5, 5 };
	AttackParam attack_sword_param;
public:
	//�_���[�W���󂯂��Ƃ��ɌĂ΂�� *�֐����ĂԂ̂̓_���[�W��^�����I�u�W�F�N�g
	AddDamageFunc damaged_function;

};
