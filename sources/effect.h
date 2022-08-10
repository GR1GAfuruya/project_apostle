#pragma once
#include <DirectXMath.h>
#include <Effekseer.h>

class Effect
{
public:
	Effect(const char* filename);
	~Effect();

	//�Đ�
	void play(const DirectX::XMFLOAT3& position, float scale = 1.0f);

	//��~
	void stop();
	//-------<setter>-------//
	//���W�ݒ�
	void set_position(const DirectX::XMFLOAT3& position);
	//�X�P�[���ݒ�
	void set_scale(const DirectX::XMFLOAT3& scale);
	//�p�x�ݒ�
	void set_angle(float angle);
	//�w�����������
	void rotate_to_target(DirectX::XMFLOAT3 target, float turn_speed = 1.0f);

	//-------<getter-------//
	//���W
	DirectX::XMFLOAT3 get_position( )const;

private:
	Effekseer::EffectRef effekseer_effect = nullptr;
	Effekseer::Handle effekseer_handle{ -1 };
};