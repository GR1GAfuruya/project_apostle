#pragma once

#include <memory>
#include <vector>
#include "constant.h"
#include "camera.h"
class Particles
{
public:
	//==============================================================
	// 
	// �\����
	// 
	//==============================================================

	struct Translation
	{
		//�ʒu
		DirectX::XMFLOAT3 position = { 0,0,0 };
		DirectX::XMFLOAT3 random_position = { 0,0,0 };

		//���x
		DirectX::XMFLOAT3 velocity = { 0,0,0 };
		DirectX::XMFLOAT3 random_velocity = { 0,0,0 };

		//�����x
		DirectX::XMFLOAT3 acceleration = { 0,0,0 };
		DirectX::XMFLOAT3 random_acceleration = { 0,0,0 };

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				cereal::make_nvp("random_position", random_position),
				cereal::make_nvp("velocity", velocity),
				cereal::make_nvp("random_velocity", random_velocity),
				cereal::make_nvp("acceleration", acceleration),
				cereal::make_nvp("random_acceleration", random_acceleration)
			);
		}


	};

	struct Scaling
	{
		//�ʒu
		DirectX::XMFLOAT3 scale = { 1,1,0 };
		DirectX::XMFLOAT3 random_scale = { 0,0,0 };

		//���x
		DirectX::XMFLOAT3 velocity = { 0,0,0 };
		DirectX::XMFLOAT3 random_velocity = { 0,0,0 };

		//�����x
		DirectX::XMFLOAT3 acceleration = { 0,0,0 };
		DirectX::XMFLOAT3 random_acceleration = { 0,0,0 };

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				cereal::make_nvp("scale", scale),
				cereal::make_nvp("random_scale", random_scale),
				cereal::make_nvp("random_velocity", random_velocity),
				cereal::make_nvp("acceleration", acceleration),
				cereal::make_nvp("random_acceleration", random_acceleration)
			);
		}


	};

	struct Rotation
	{
		//�p�x
		DirectX::XMFLOAT3 rotation = { 0,0,0 };
		DirectX::XMFLOAT3 random_rotation = { 0,0,0 };
		//�p���x
		DirectX::XMFLOAT3 velocity = { 0,0,0 };
		DirectX::XMFLOAT3 random_velocity = { 0,0,0 };

		//�p�����x
		DirectX::XMFLOAT3 acceleration = { 0,0,0 };
		DirectX::XMFLOAT3 random_acceleration = { 0,0,0 };

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				cereal::make_nvp("rotation", rotation),
				cereal::make_nvp("random_rotation", random_rotation),
				cereal::make_nvp("random_velocity", random_velocity),
				cereal::make_nvp("acceleration", acceleration),
				cereal::make_nvp("random_acceleration", random_acceleration)
			);
		}
	};

	struct Color
	{
		//�F
		DirectX::XMFLOAT4 color = { 0,0,0,0 };
		DirectX::XMFLOAT4 random_color = { 0,0,0,0 };
	};

	struct Param
	{
		//�ʒu�n
		Translation transition;
		//��]
		Rotation rotate;
		//�傫��
		Scaling scaling;

		DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
		//�F
		DirectX::XMFLOAT4 color{ 1,1,1,1 };
		//�p�x
		DirectX::XMFLOAT3 init_angle = { 0,0,0 };
		//��������Ă���̎���
		float time;
		//����
		float life_time = 10;


	};
	//==============================================================
	// 
	// �R���X�g���N�^�E�f�X�g���N�^
	// 
	//==============================================================
	Particles(Param init_param);
	Particles(const Particles&) = delete;
	Particles& operator=(const Particles&) = delete;
	Particles(Particles&&) noexcept = delete;
	Particles& operator=(Particles&&) noexcept = delete;
	~Particles();
	//==============================================================
	// 
	// �p�u���b�N�֐�
	// 
	//==============================================================
	void initialize();
	void update(float elapsed_time);
	//==============================================================
	// 
	// �Z�b�^�[�ƃQ�b�^�[
	// 
	//==============================================================
	//�ʒu�擾
	const DirectX::XMFLOAT3& get_position() const { return param.transition.position; }
	//�ʒu�ݒ�
	void set_position(const DirectX::XMFLOAT3& position) { param.transition.position = position; }
	// ��]�擾
	const DirectX::XMFLOAT3& get_angle() const { return param.rotate.rotation; }
	// ��������Ă���̎��Ԏ擾
	const float& get_time() const { return param.time; }
	// �����Ɛ�������Ă���̎��Ԃ̊���
	const float& get_life_rate() const { return param.time / param.life_time; }

	//��]�ݒ�
	void set_angle(DirectX::XMFLOAT3& ang) { param.rotate.rotation = ang; }
	// �X�P�[���擾
	const DirectX::XMFLOAT3& get_scale() const { return param.scaling.scale; }
	//�X�P�[���ݒ�
	void set_scale(const DirectX::XMFLOAT3& scale) { param.scaling.scale = scale; }
	//�X�P�[���ݒ�
	void set_scale(const DirectX::XMFLOAT2& scale) { param.scaling.scale = { scale.x, scale.y, 1 }; }
	//Orientation�擾
	const DirectX::XMFLOAT4& get_orientation() const { return param.orientation; }
	//color�擾
	const DirectX::XMFLOAT4& get_color() const { return param.color; }
	//color�ݒ�
	void set_color(const DirectX::XMFLOAT4& color) { param.color = color; }

	const bool get_is_active() { return is_active; }

	void look_at_camera(Camera& camera);

private:
	//�ʒu�X�V
	void position_update(float elapsed_time);
	//�p���X�V
	void orientation_update(float elapsed_time);
	//�����X�V
	void life_update(float elapsed_time);

	Param param;
	bool is_active;

	float age = 0;

};