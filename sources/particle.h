#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <memory>
#include <vector>
#include "constant.h"
#include "graphics.h"
#include "sprite_batch.h"
class Particles
{
public:
	struct InitParam
	{
		//�ʒu
		DirectX::XMFLOAT3 position = { 0,0,0 };
		//�ˏo����
		DirectX::XMFLOAT3 velocity = { 0,0,0 };
		//�傫��
		DirectX::XMFLOAT2 scale = { 0,0 };
		//�F
		DirectX::XMFLOAT4 color{ 1,1,1,1 };
		//����
		float life_time = FLT_MAX;

		
	};


	Particles(Graphics& graphics, InitParam init_param);
	Particles(const Particles&) = delete;
	Particles& operator=(const Particles&) = delete;
	Particles(Particles&&) noexcept = delete;
	Particles& operator=(Particles&&) noexcept = delete;
	~Particles();


	//�ʒu�擾
	const DirectX::XMFLOAT3& get_position() const { return position; }
	//�ʒu�ݒ�
	void set_position(const DirectX::XMFLOAT3& position) { this->position = position; }
	// ��]�擾
	const float& get_angle() const { return angle; }
	//��]�ݒ�
	void set_angle(float angle) { this->angle = angle; }
	// �X�P�[���擾
	const DirectX::XMFLOAT2& get_scale() const { return scale; }
	//velocity�擾
	const DirectX::XMFLOAT3& get_velocity() const { return velocity; }
	//�X�P�[���ݒ�
	void set_scale(const DirectX::XMFLOAT2& scale) { this->scale = scale; }
	//velocity�Z�b�g
	void set_velocity(const DirectX::XMFLOAT3& v) { this->velocity = v; }
	void initialize(Graphics& graphics);
	void update(Graphics& graphics, float elapsed_time);
	//color�擾
	const DirectX::XMFLOAT4& get_color() const { return color; }
	//color�ݒ�
	void set_scale(const DirectX::XMFLOAT4& color) { this->color = color; }

	const bool get_is_active() { return is_active; }
private:
	void position_update(float elapsed_time);
	void life_update(float elapsed_time);

	DirectX::XMFLOAT2 scale = { 5,5 };
	DirectX::XMFLOAT3 rotate{ 0,0,0 };
	DirectX::XMFLOAT3 position{ 0,0,0 };
	DirectX::XMFLOAT3 velocity{ 0,0,0};
	DirectX::XMFLOAT4 color{ 1,1,1,1 };
	float angle = 0;
	bool is_active;

	float time = 0;
	float life_time = 0;



};