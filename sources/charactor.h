#pragma once

#include <DirectXMath.h>
#include "move_behavior.h"
#include "generic_frequent_entities.h"
#include <functional>

typedef std::function<void(int, float)> AddDamageFunc;

//�L�����N�^�[
class Charactor :public MoveBehavior,public GenericFrequentEntities
{
public:
	Charactor() {}
	virtual ~Charactor(){}
	//�s��X�V����
	void update_transform();
	//�s��X�V����
	//�����������_�ɂ��炷�l
	//��O���������炵����̈ړ��l
	void update_transform(const DirectX::XMFLOAT4& orien, const DirectX::XMFLOAT3& pos = {1,1,1});
	//�������͍X�V����
	void update_vertical_velocity(float elapsed_flame) override;
	//�����ړ��X�V����
	void update_vertical_move(float elapsed_time, DirectX::XMFLOAT3& position, Stage* stage)override;
	//�������͍X�V����
	void update_hrizontal_velocity(float elapsed_time) override;
	//�����ړ��X�V����
	void update_horizontal_move(float elapsed_time, DirectX::XMFLOAT3& position, Stage* stage) override;

	//�s��X�V����
//	void update_transform(const DirectX::XMFLOAT4& orien, const DirectX::XMFLOAT3& pos = {1,1,1}, const DirectX::XMFLOAT4X4& transform = {});
	//�ʒu�擾
	const DirectX::XMFLOAT3& get_position() const { return position; }
	//�ʒu�ݒ�
	void set_position(const DirectX::XMFLOAT3& position) { this->position = position; }
	// ��]�擾
	const DirectX::XMFLOAT3 & get_angle() const { return angle; }
	//��]�ݒ�
	void set_angle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
	// �X�P�[���擾
	const DirectX::XMFLOAT3 & get_scale() const { return scale; }
	//velocity�擾
	const DirectX::XMFLOAT3& get_velocity() const { return velocity; }
	//�X�P�[���ݒ�
	void set_scale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	//velocity�Z�b�g
	void set_velocity(const DirectX::XMFLOAT3& v) { this->velocity = v; }
	// ���a
	float get_radius() const {return radius;}
	// HP
	int get_health() const { return health; }
	// �ő�HP
	uint32_t GetMaxHealth() const { return max_health; }
	// �n�ʔ���
	bool get_is_ground() const { return is_ground; }
	//�����擾
	float get_height() const { return height; }
	//�g�����X�t�H�[���̃Q�b�^�[
	const DirectX::XMFLOAT4X4& get_transform() const { return transform; }
	//�Ռ���^����
	void add_impulse(const DirectX::XMFLOAT3& impulse);
	//�_���[�W��^����
	bool apply_damage(int damage, float invincibleTime);

protected:
	//void Move(float elpsedTime, float vx, float vz, float speed);
	void Move(float vx, float vz, float speed);
	void Turn(float elapsed_time, float vx, float vz, float speed);//�I�C���[
	void Turn(float elapsed_time, DirectX::XMFLOAT3 move_vec, float speed, DirectX::XMFLOAT4& orien);//�N�H�[�^�j�I��
	//�W�����v����
	void Jump(float speed);
	//���͏����X�V
	void update_velocity(float elapsed_time, DirectX::XMFLOAT3& position, Stage* stage);
	virtual void on_landing() {}
	//���S�����Ƃ��ɌĂ΂��
	virtual void OnDead() {}
	void update_invicible_timer(float elapsed_time);

	

//-----------�ϐ�--------------//

	DirectX::XMFLOAT3	position = { 0, 0, 0 };
	DirectX::XMFLOAT3	angle = { 0, 0, 0 };
	DirectX::XMFLOAT3	scale = { 1, 1, 1 };
	DirectX::XMFLOAT4 orientation{0,0,0,1};
	DirectX::XMFLOAT4X4	transform = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	};


protected:
	float stepOffset = 0.7f;
	//�L�����̓��̂Ă��؂�
	float head_top = 1.0f;
	float radius = 1.0f;
	//�d��
	float gravity = -1.0;
	DirectX::XMFLOAT3 velocity = { 0, 0, 0 };
	//�n�ʂɓ������Ă��邩
	bool is_ground = false;
	//�L�����̏c�̑傫��
	float height = 2.6f;
	//�̗�
	int32_t health = 1000;
	int32_t max_health = 1000;

	float invincible_timer = 0.0f;
	//���C��
	float friction = 1.0f;
	//�����x
	float acceleration = 1.5f;
	float max_move_speed = 30.0f;
	float move_vec_x = 0.0f;
	float move_vec_z = 0.0f;

	float air_control = 0.3f;

	//�@��
	DirectX::XMFLOAT3 slope_normal = {};

	

};
