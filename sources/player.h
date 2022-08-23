#pragma once
#include "framework.h"
#include "camera.h"
#include "player_move.h"
#include "skeletal_mesh.h"
#include "aura.h"
#include "gpu_particle.h"
#include "slash.h"
//#include	"Effect.h"
//�v���C���[ :final ���̃N���X�̌p�����ł��Ȃ����Ƃ𖾎�����
class Player final :public PlayerMove
{
public:
	Player(Graphics& graphics, Camera* camera);
	~Player()override;

	//����������
	void initialize();
	//�X�V����
	void update(Graphics& graphics, float elapsed_time, Camera* camera,Stage* stage);
	//�`�揈��
	//�f�B�t�@�[�h�Ń����_�����O����I�u�W�F�N�g
	void render_d(Graphics& graphics, float elapsed_time, Camera* camera);
	//�t�H���[�h�����_�����O����I�u�W�F�N�g
	void render_f(Graphics& graphics, float elapsed_time, Camera* camera);
	//�f�o�b�O�pGUI�`��
	void debug_gui();

	

/////////////�A�j���[�V����////////////////
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
	DirectX::XMFLOAT3 get_camera_target_pos() { return DirectX::XMFLOAT3(position.x, position.y + height, position.z); }


	//--------�萔--------//
	 //�U��1���ڂ̗P�\����
	static constexpr float ATTACK_TYPE1_MAX_TIME = 0.3f;
	//�U��2���ڂ̗P�\����
	static constexpr float ATTACK_TYPE2_MAX_TIME = 0.2f;
	//�U��3���ڂ̗P�\����
	static constexpr float ATTACK_TYPE3_MAX_TIME = 0.2f;



private:
	//////�J��
	void transition_idle_state();
	void transition_attack_state();
	void transition_attack_combo1_state();
	void transition_attack_combo2_state();
	void transition_attack_combo3_state();
	void transition_move_state();
	void transition_jump_state();
	//void transition_fall_state();
	//void transition_landing_state();


	//////�A�j���[�V�����A�b�v�f�[�g
	void update_idle_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_state(Graphics& graphics, float elapsed_time, Camera* camera, Stage* stage);
	void update_attack_combo1_state(Graphics& graphics, float elapsed_time, Camera* camera,Stage* stage);
	void update_attack_combo2_state(Graphics& graphics, float elapsed_time, Camera* camera,Stage* stage);
	void update_attack_combo3_state(Graphics& graphics, float elapsed_time, Camera* camera,Stage* stage);
	void update_move_state(Graphics& graphics, float elapsed_time, Camera* camera,Stage* stage);
	void update_jump_state(Graphics& graphics, float elapsed_time, Camera* camera,Stage* stage);
	//void update_fall_state(float elapsed_time, Camera* camera);
	//void update_landing_state(float elapsed_time, Camera* camera);

	typedef void (Player::* p_Update)(Graphics& graphics,float elapsed_time, Camera* camera, Stage* stage);
	p_Update p_update = &Player::update_idle_state;

	void Attack(Graphics& graphics, float elapsed_time);
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;
		
	std::unique_ptr<Aura> aura = nullptr;

	State state;

	GamePad* game_pad;
	Mouse* mouse;
	/*GamePad& game_pad;
	Mouse& mouse ;*/
	DirectX::XMFLOAT4X4 world;
	//�v���C���[�̈ړ����͏���
	bool input_move(float elapsedTime, Camera* camera);
	const DirectX::XMFLOAT3 get_move_vec(Camera* camera) const;
	//�W�����v���͏���
	void input_jump();

	//�I�u�W�F�N�g������
protected:
	void on_landing()override;

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

	//�U������
	float attack_time;

	std::unique_ptr<GPU_Particles> attack1;
	std::unique_ptr<Slash> slash_efect;
	skeleton::bone sword_hand;
	
	bool display_player_imgui = false;
/// </summary>
	//�����蔻��p�ϐ�
	DirectX::XMFLOAT3 radius_aabb = { 5, 5, 5 };
};
