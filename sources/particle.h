#pragma once

#include <memory>
#include <vector>
#include "constant.h"
#include "camera.h"
#include "transform.h"
class Particles
{
public:
	//==============================================================
	// 
	// 構造体
	// 
	//==============================================================

	struct Translation
	{
		//位置
		DirectX::XMFLOAT3 position = { 0,0,0 };
		//速度
		DirectX::XMFLOAT3 velocity = { 0,0,0 };
		//加速度
		DirectX::XMFLOAT3 acceleration = { 0,0,0 };


	};

	struct Scaling
	{
		//位置
		DirectX::XMFLOAT3 scale = { 1,1,0 };

		//速度
		DirectX::XMFLOAT3 velocity = { 0,0,0 };

		//加速度
		DirectX::XMFLOAT3 acceleration = { 0,0,0 };


	};

	struct Rotation
	{
		//角度
		DirectX::XMFLOAT3 rotation = { 0,0,0 };
		//角速度
		DirectX::XMFLOAT3 velocity = { 0,0,0 };

		//角加速度
		DirectX::XMFLOAT3 acceleration = { 0,0,0 };
	};

	struct Color
	{
		//色
		DirectX::XMFLOAT4 color = { 0,0,0,0 };
	};

	struct Param
	{
		//位置系
		Translation transition = {};
		//回転
		Rotation rotate = {};
		//大きさ
		Scaling scaling = {};

		DirectX::XMFLOAT4 orientation = { 0,0,0,1 };

		DirectX::XMFLOAT4X4	transform = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		//色
		DirectX::XMFLOAT4 color{ 1,1,1,1 };
		//角度
		DirectX::XMFLOAT3 init_angle = { 0,0,0 };
		//生成されてからの時間
		float time = 0;
		//寿命
		float life_time = 10;


	};
	//==============================================================
	// 
	// コンストラクタ・デストラクタ
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
	// パブリック関数
	// 
	//==============================================================
	void initialize();
	void update(float elapsed_time);
	//==============================================================
	// 
	// セッターとゲッター
	// 
	//==============================================================
	//位置取得
	const DirectX::XMFLOAT3& get_position() const { return param.transition.position; }
	//位置設定
	void set_position(const DirectX::XMFLOAT3& position) { param.transition.position = position; }
	// 回転取得
	const DirectX::XMFLOAT3& get_angle() const { return param.rotate.rotation; }
	// 生成されてからの時間取得
	const float& get_time() const { return param.time; }
	// 寿命と生成されてからの時間の割合
	const float& get_life_rate() const { return param.time / param.life_time; }

	//回転設定
	void set_angle(DirectX::XMFLOAT3& ang) { param.rotate.rotation = ang; }
	// スケール取得
	const DirectX::XMFLOAT3& get_scale() const { return param.scaling.scale; }
	//スケール設定
	void set_scale(const DirectX::XMFLOAT3& scale) { param.scaling.scale = scale; }
	//スケール設定
	void set_scale(const DirectX::XMFLOAT2& scale) { param.scaling.scale = { scale.x, scale.y, 1 }; }
	//Orientation取得
	const DirectX::XMFLOAT4& get_orientation() const { return param.orientation; }
	//color取得
	const DirectX::XMFLOAT4& get_color() const { return param.color; }
	//color設定
	void set_color(const DirectX::XMFLOAT4& color) { param.color = color; }

	const bool get_is_active() { return is_active; }

	void look_at_camera(Camera& camera);

	Param param;
private:
	//位置更新
	void position_update(float elapsed_time);
	//姿勢更新
	void orientation_update(float elapsed_time);
	//寿命更新
	void life_update(float elapsed_time);

	
	bool is_active;

	float age = 0;

};