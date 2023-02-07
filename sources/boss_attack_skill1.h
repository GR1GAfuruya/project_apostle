#pragma once
#include "mesh_effect.h"
#include "gpu_particle.h"
#include "meteore.h"
#include "light.h"
#include "camera.h"
#include <functional>
class BossAttackSkill1
{
	//==============================================================
	// 
	// public�֐�
	// 
	//=============================================================
public:

	struct WaveParam
	{
		DirectX::XMFLOAT3 scale;
		float scale_speed;
		bool is_hit = false;
		Ring colider_ring;

	};
	BossAttackSkill1(Graphics& graphics);
	~BossAttackSkill1();

	//����
	void chant(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir);
	//��~
	void stop();
	//�X�V
	void update(Graphics& graphics, float elapsed_time, Camera* camera, DirectX::XMFLOAT3 arm_pos, DirectX::XMFLOAT3 arm_dir);
	//�`��
	void render(Graphics& graphics, Camera* camera);
	//�f�o�b�OGUI
	void debug_gui(const char* str_id);



	//�����蔻��
	void calc_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func);


private:

	void none_update(Graphics& graphics, float elapsed_time, Camera* camera);
	//�`���[�W���̍X�V
	void charge_state_update(Graphics& graphics, float elapsed_time, Camera* camera);
	//�U�����̍X�V
	void attack_state_update(Graphics& graphics, float elapsed_time, Camera* camera);

	typedef std::function<void(Graphics& graphics, float elapsed_time, Camera* camera)> StateUpdate;

	StateUpdate state_update;
	float ray_power = 5.0f;
	std::unique_ptr<MeshEffect> arm_effect;
	std::unique_ptr<MeshEffect>wave;
	WaveParam params;

	std::unique_ptr<GPU_Particles> spark_effect;
	//GPU�p�[�e�B�N���̃G�~�b�^�[CS
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	//GPU�p�[�e�B�N���̃A�b�v�f�[�gCS
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;


	DirectX::XMFLOAT3 arm_pos;
	DirectX::XMFLOAT3 arm_dir;
	float range;
	float charge_timer;
	float charge_time;
	float skill_time;
	AttackParam at_param;

	bool display_imgui = false;
	//�`���[�W���̃J�����V�F�C�N
	Camera::CameraShakeParam camera_shake = {};

};