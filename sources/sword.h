#pragma once
#include "graphics.h"
#include "skeletal_mesh.h"
#include "user.h"
class Sword
{
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	Sword() {}
	Sword(Graphics& graphics);
	~Sword() {}

	struct Capsule
	{
		DirectX::XMFLOAT3 start;
		DirectX::XMFLOAT3 end;
		float radius;
	};

	void initialize();
	void update(Graphics& graphics, float elapsed_time);
	void render(Graphics& graphics);

	//�������̈ʒu�擾
	const DirectX::XMFLOAT3& get_equipped_position() const { return DirectX::XMFLOAT3(transform._41, transform._42, transform._43); }
	//����ւ̃x�N�g���擾
	const DirectX::XMFLOAT3& get_dir_sword_top() const { return Math::get_posture_forward(transform); }
	const Capsule& get_collision() const { return collision; };
	//�ʒu�ݒ�
	void set_position(const DirectX::XMFLOAT3& position) { this->position = position; }
	//���̌������{�[���̃N�H�[�^�j�I���ɍ��킹��
	void set_parent_transform(const DirectX::XMFLOAT4X4& parent_transform) { this->parent_transform = parent_transform; }
private:

	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================

	DirectX::XMFLOAT3	position = { 0, 0, 0 };
	DirectX::XMFLOAT3	scale = { 1, 1, 1 };
	DirectX::XMFLOAT4 orientation{ 0,0,0,1 };
	DirectX::XMFLOAT4X4	transform = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	};

	DirectX::XMFLOAT4X4	parent_transform = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	};

	// �X�P���^�����b�V���̎���
	std::unique_ptr <SkeletalMesh> model;
	Capsule collision;

	//���̒���
	float length;
	//���̑���
	float radius;
};
