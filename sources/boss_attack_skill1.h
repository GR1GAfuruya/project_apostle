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
	BossAttackSkill1(Graphics& graphics);
	~BossAttackSkill1() {}

	//����
	void chant(DirectX::XMFLOAT3 pos);
	//��~
	void stop();
	//�X�V
	void update(Graphics& graphics, float elapsed_time, Camera* camera);
	//�`��
	void render(Graphics& graphics);
	//�f�o�b�OGUI
	void debug_gui(const char* str_id);

	void set_hand_pos(DirectX::XMFLOAT3 pos) { hand_pos = pos; }


	//�����蔻��
	void calc_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func);

private:
	//�`���[�W���̍X�V
	void charge_state_update(Graphics& graphics, float elapsed_time, Camera* camera);
	//�U�����̍X�V
	void attack_state_update(Graphics& graphics, float elapsed_time, Camera* camera);

	typedef std::function<void(Graphics& graphics, float elapsed_time, Camera* camera)> StateUpdate;

	StateUpdate state_update;

	std::unique_ptr<GPU_Particles> spark_effect;
	//GPU�p�[�e�B�N���̃G�~�b�^�[CS
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	//GPU�p�[�e�B�N���̃A�b�v�f�[�gCS
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;

	float range;

	DirectX::XMFLOAT3 hand_pos;

};