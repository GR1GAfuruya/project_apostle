#pragma once
#include "mesh_effect.h"
#include "gpu_particle.h"
#include "meteore.h"
#include "light.h"
#include "camera.h"
#include <functional>
class BossAttackSkill3 :public MoveBehavior
{
	//==============================================================
	// 
	// public�֐�
	// 
	//=============================================================
public:

	struct MeteoreParam
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 velocity;
		DirectX::XMFLOAT3 move_vec;
		DirectX::XMFLOAT3 scale;
		bool is_calc_velocity = false;
		bool is_hit = false;
		Sphere colider_sphere;

	};
	BossAttackSkill3(Graphics& graphics);
	~BossAttackSkill3();

	//����
	void chant(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir);
	//��~
	void stop();
	//�X�V
	void update(Graphics& graphics, float elapsed_time, Camera* camera, DirectX::XMFLOAT3 arm_pos, DirectX::XMFLOAT3 arm_dir);
	//�`��
	void render(Graphics& graphics);
	//�f�o�b�OGUI
	void debug_gui(const char* str_id);



	//�����蔻��
	void calc_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func);


private:
	void update_velocity(float elapsed_time, int index);
	//�ړ��Ɋւ���p�����[�^�[���Z�b�g
	void move(float vx, float vz, float speed, int index);
	//�������͍X�V����
	void update_vertical_velocity(float elapsed_frame, int index);
	//�����ړ��X�V����
	void update_vertical_move(float elapsed_time, int index);
	//�������͍X�V����
	void update_hrizontal_velocity(float elapsed_frame, int index);
	//�����ړ��X�V����
	void update_horizontal_move(float elapsed_time, int index);
	//�e�������������̏���
	void on_hit(int index);

	//�`���[�W���̍X�V
	void charge_state_update(Graphics& graphics, float elapsed_time, Camera* camera);
	//�U�����̍X�V
	void attack_state_update(Graphics& graphics, float elapsed_time, Camera* camera);

	typedef std::function<void(Graphics& graphics, float elapsed_time, Camera* camera)> StateUpdate;

	StateUpdate state_update;
	float ray_power = 5.0f;
	std::unique_ptr<InstanceMeshEffect> meteore_effect;
	std::unique_ptr<MeshEffect> arm_effect;
	std::unique_ptr<MeshEffect> meteo_wave[3];
	std::unique_ptr<MeteoreParam[]> params;

	std::unique_ptr<GPU_Particles> spark_effect;
	//GPU�p�[�e�B�N���̃G�~�b�^�[CS
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	//GPU�p�[�e�B�N���̃A�b�v�f�[�gCS
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;

	DirectX::XMFLOAT3 skill_dir[3];

	DirectX::XMFLOAT3 arm_pos;
	DirectX::XMFLOAT3 arm_dir;
	float range;
	float charge_timer;
	float charge_time;
	static const int MAX_NUM = 3;
	AttackParam at_param;
};