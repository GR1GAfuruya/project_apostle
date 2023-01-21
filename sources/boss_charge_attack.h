#pragma once
#include "mesh_effect.h"
#include "gpu_particle.h"
#include "meteore.h"
#include "light.h"
#include "camera.h"
class ChargeAttack
{
public:
	//�R���X�g���N�^
	ChargeAttack(Graphics& grapghics);
	//�f�X�g���N�^
	~ChargeAttack(){}
	//����
	void chant(DirectX::XMFLOAT3 pos);
	//��~
	void stop();
	//�X�V
	void update(Graphics& graphics, float elapsed_time,Camera* camera);
	//�`��
	void render(Graphics& graphics);
	//�f�o�b�OGUI
	void debug_gui(const char* str_id);
	//�`���[�W�����������ǂ���
	void set_charge_max_state() { is_charge_max = true; }
	//�U���Ώېݒ�
	void set_target_pos(DirectX::XMFLOAT3 target) { target_pos = target; }
	//�����蔻��
	void calc_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func);

private:
	//�`���[�W���̍X�V
	void charging_update(Graphics& graphics, float elapsed_time, Camera* camera);
	//�������̍X�V
	void activities_update(Graphics& graphics, float elapsed_time, Camera* camera);
	//���Ŏ��̍X�V
	void vanishing_update(Graphics& graphics, float elapsed_time, Camera* camera);

	typedef void (ChargeAttack::* ChargeAttackUpdate)(Graphics& graphics, float elapsed_time, Camera* camera);
	ChargeAttackUpdate charge_attack_update = &ChargeAttack::charging_update;
	struct ChargeAttackConstants
	{
		DirectX::XMFLOAT3 core_pos{};
		float core_gravitation = 0.5;
		DirectX::XMFLOAT3 particle_sub_color{};
		float core_radius{};
	};
	//�R�A
	std::unique_ptr<MeshEffect> core;
	//�g��
	std::unique_ptr<MeshEffect> wave;
	//�g���l�[�h
	std::unique_ptr<MeshEffect> tornado;
	std::unique_ptr<MeshEffect> tornado_black;

	//�\���G�t�F�N�g
	std::unique_ptr<MeshEffect> omen_effect;
	//GPU�p�[�e�B�N���̃G�~�b�^�[CS
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	//GPU�p�[�e�B�N���̃A�b�v�f�[�gCS
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;
	//GPU�p�[�e�B�N��
	std::unique_ptr<GPU_Particles> particle;
	//���e�I
	unique_ptr<Meteore> meteores;
	float meteo_time = 0;
	float meteo_span = 0;
	float meteo_launch_radius = 0;
	int meteo_launch_count = 0;
	//---<PointLight>-----//
	//
	shared_ptr<PointLight> boss_light;

	//�萔�o�b�t�@
	std::unique_ptr<Constants<ChargeAttackConstants>> constants;
	const float ATTACK_TIME = 3.0f;
	bool is_charge_max = false;
	float attack_time = 0.0f;
	//�ʒu
	DirectX::XMFLOAT3 position = { 0,0,0 };
	//��]��
	DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
	//�X�P�[��
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	static constexpr DirectX::XMFLOAT4 FIRE_COLOR = { 4.0f, 1.0f, 0.7f, 0.8f };
	//�f�o�b�OGUI�t���O
	bool display_imgui = false;

	//��������
	float life_time = 0;
	//�A�N�e�B�u��Ԃ�
	bool active = false;

	//�U���Ώۂ̍��W
	DirectX::XMFLOAT3 target_pos;
	AttackParam attack_param;
	Sphere tornado_colider;
};