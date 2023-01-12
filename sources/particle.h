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
		//位置
		DirectX::XMFLOAT3 position = { 0,0,0 };
		//射出方向
		DirectX::XMFLOAT3 velocity = { 0,0,0 };
		//大きさ
		DirectX::XMFLOAT2 scale = { 0,0 };
		//色
		DirectX::XMFLOAT4 color{ 1,1,1,1 };
		//寿命
		float life_time = FLT_MAX;

		
	};


	Particles(Graphics& graphics, InitParam init_param);
	Particles(const Particles&) = delete;
	Particles& operator=(const Particles&) = delete;
	Particles(Particles&&) noexcept = delete;
	Particles& operator=(Particles&&) noexcept = delete;
	~Particles();


	//位置取得
	const DirectX::XMFLOAT3& get_position() const { return position; }
	//位置設定
	void set_position(const DirectX::XMFLOAT3& position) { this->position = position; }
	// 回転取得
	const float& get_angle() const { return angle; }
	//回転設定
	void set_angle(float angle) { this->angle = angle; }
	// スケール取得
	const DirectX::XMFLOAT2& get_scale() const { return scale; }
	//velocity取得
	const DirectX::XMFLOAT3& get_velocity() const { return velocity; }
	//スケール設定
	void set_scale(const DirectX::XMFLOAT2& scale) { this->scale = scale; }
	//velocityセット
	void set_velocity(const DirectX::XMFLOAT3& v) { this->velocity = v; }
	void initialize(Graphics& graphics);
	void update(Graphics& graphics, float elapsed_time);
	//color取得
	const DirectX::XMFLOAT4& get_color() const { return color; }
	//color設定
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