#pragma once
#include "graphics.h"
#include "constant.h"
class Light
{
public:
	Light();
	~Light() {}

	//-------<関数>-------//
	virtual void debug_gui(std::string name) {};
protected:
	//定数バッファ
	struct DirLightParam {
		DirectX::XMFLOAT4 dir_light_dir;
		DirectX::XMFLOAT4 dir_light_color;
	};

public:
	std::unique_ptr<Constants<DirLightParam>> light_constants{};
	std::string name;
};

//平行光
class DirectionalLight :public Light
{
public:
	DirectionalLight(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 color);
	~DirectionalLight() {}

	void debug_gui(std::string name) override;

	void set_direction(DirectX::XMFLOAT3 position);
	void set_color(DirectX::XMFLOAT3 color);

	DirectX::XMFLOAT3 get_direction();
	DirectX::XMFLOAT3 get_color();
};

//点光源
class PointLight :public Light
{
public:
	PointLight(DirectX::XMFLOAT3 position, float distance, DirectX::XMFLOAT3 color);
	~PointLight() {}

	void debug_gui(std::string name) override;

	void set_position(DirectX::XMFLOAT3 position);
	void set_color(DirectX::XMFLOAT3 color);
	void set_distance(float d);

	DirectX::XMFLOAT3 get_position();
	DirectX::XMFLOAT3 get_color();
	float get_distance();
};