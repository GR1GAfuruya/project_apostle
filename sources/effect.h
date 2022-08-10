#pragma once
#include <DirectXMath.h>
#include <Effekseer.h>

class Effect
{
public:
	Effect(const char* filename);
	~Effect();

	//再生
	void play(const DirectX::XMFLOAT3& position, float scale = 1.0f);

	//停止
	void stop();
	//-------<setter>-------//
	//座標設定
	void set_position(const DirectX::XMFLOAT3& position);
	//スケール設定
	void set_scale(const DirectX::XMFLOAT3& scale);
	//角度設定
	void set_angle(float angle);
	//指定方向を向く
	void rotate_to_target(DirectX::XMFLOAT3 target, float turn_speed = 1.0f);

	//-------<getter-------//
	//座標
	DirectX::XMFLOAT3 get_position( )const;

private:
	Effekseer::EffectRef effekseer_effect = nullptr;
	Effekseer::Handle effekseer_handle{ -1 };
};