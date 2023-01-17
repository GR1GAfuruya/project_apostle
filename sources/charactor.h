#pragma once

#include <DirectXMath.h>
#include "move_behavior.h"
#include "generic_frequent_entities.h"

#include "damage_func.h"

//�L�����N�^�[
class Charactor :public GenericFrequentEntities
{
	
public:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================
	struct CharactorParam
	{
		//�L�����N�^�[�̔��a
		float radius = 1.0f;
		//�L�����̏c�̑傫��
		float height = 2.6f;
		//�ő�̗�
		int max_health = 1000;
		//���C��
		float friction = 1.0f;
		//��C��R
		float air_control = 0.3f;
		//�����x
		float acceleration = 1.5f;
		//�ő呬�x
		float max_move_speed = 30.0f;
		//��]���x
		float turn_speed;
		//�ړ����x
		float move_speed;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				cereal::make_nvp("radius", radius),
				cereal::make_nvp("height", height),
				cereal::make_nvp("max_health", max_health),
				cereal::make_nvp("friction", friction),
				cereal::make_nvp("air_control", air_control),
				cereal::make_nvp("acceleration", acceleration),
				cereal::make_nvp("max_move_speed", max_move_speed),
				cereal::make_nvp("turn_speed", turn_speed),
				cereal::make_nvp("move_speed", move_speed)
			);
		}

	};

	//==============================================================
	// 
	// �R���X�g���N, �f�X�g���N�^
	// 
	//==============================================================
	Charactor() {}
	virtual ~Charactor() {}

	//==============================================================
	// 
	// �Z�b�^�[�ƃQ�b�^�[
	// 
	//==============================================================

	//�ʒu�擾
	const DirectX::XMFLOAT3& get_position() const { return position; }
	//�ʒu�ݒ�
	void set_position(const DirectX::XMFLOAT3& position) { this->position = position; }
	// ��]�擾
	const DirectX::XMFLOAT3& get_angle() const { return angle; }
	//��]�ݒ�
	void set_angle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
	// �X�P�[���擾
	const DirectX::XMFLOAT3& get_scale() const { return scale; }
	//velocity�擾
	const DirectX::XMFLOAT3& get_velocity() const { return velocity; }
	//�X�P�[���ݒ�
	void set_scale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	//velocity�Z�b�g
	void set_velocity(const DirectX::XMFLOAT3& v) { this->velocity = v; }
	// ���a
	float get_radius() const { return chara_param.radius; }
	// HP
	int get_health() const { return health; }
	// �ő�HP
	int get_max_health() const { return chara_param.max_health; }
	//�̂̐��ʂƐi�s���ʂƂ̊p�x
	float get_turn_angle() const { return turn_angle; }
	//HP�p�[�Z���e�[�W
	float get_hp_percent() const { return health <= 0 ? 0.0f : static_cast<float>(health) / static_cast<float>(chara_param.max_health); }
	// �n�ʔ���
	bool get_is_ground() const { return is_ground; }
	//�����擾
	float get_height() const { return chara_param.height; }
	//�g�����X�t�H�[���̃Q�b�^�[
	const DirectX::XMFLOAT4X4& get_transform() const { return transform; }
	//�Ռ���^����
	void add_impulse(const DirectX::XMFLOAT3& impulse);
	//�_���[�W��^����
	bool apply_damage(int damage, float invincibleTime, WINCE_TYPE type);

protected:

	void Move(float vx, float vz, float speed);
	void Turn(float elapsed_time, float vx, float vz, float speed);//�I�C���[
	void Turn(float elapsed_time, DirectX::XMFLOAT3 move_vec, float speed, DirectX::XMFLOAT4& orien);//�N�H�[�^�j�I��
	//�W�����v����
	void Jump(float speed);
	//���͏����X�V
	void update_velocity(float elapsed_time, DirectX::XMFLOAT3& position);
	virtual void on_landing() {}
	//���S�����Ƃ��ɌĂ΂��
	virtual void on_dead() {}
	virtual void on_damaged(WINCE_TYPE type) {}
	void update_invicible_timer(float elapsed_time);



	//-----------�ϐ�--------------//

	DirectX::XMFLOAT3	position = { 0, 0, 0 };
	DirectX::XMFLOAT3	angle = { 0, 0, 0 };
	DirectX::XMFLOAT3	scale = { 1, 1, 1 };
	DirectX::XMFLOAT4 orientation{ 0,0,0,1 };
	DirectX::XMFLOAT4X4	transform = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	};

	CharactorParam chara_param;

	float stepOffset = 0.7f;
	DirectX::XMFLOAT3 velocity = { 0, 0, 0 };
	//�n�ʂɓ������Ă��邩
	bool is_ground = false;

	float invincible_timer = 0.0f;
	float move_vec_x = 0.0f;
	float move_vec_z = 0.0f;

	//�̗�
	int32_t health;


	float vs_wall_ray_power = 5.0f;
	//��̖@��
	DirectX::XMFLOAT3 slope_normal = {};
	float gravity = -1.0f;
	float turn_angle;			//�̂�������Ƃ��̉�]�p
	//-----------�v���C�x�[�g�֐�--------------//
private:
	//�������͍X�V����
	void update_vertical_velocity(float elapsed_flame) ;
	//�����ړ��X�V����
	void update_vertical_move(float elapsed_time, DirectX::XMFLOAT3& position);
	//�������͍X�V����
	void update_hrizontal_velocity(float elapsed_frame) ;
	//�����ړ��X�V����
	void update_horizontal_move(float elapsed_time, DirectX::XMFLOAT3& position) ;
};
