#pragma once
#include "graphics.h"
#include "constant.h"
class Light
{
public:
	Light(Graphics& graphics);
	~Light(){}

	//-------<関数>-------//
	virtual void debug_gui(int unique_id){};
protected:
	//定数バッファ
	struct DirLightParam {
		DirectX::XMFLOAT4 dir_light_dir;
		DirectX::XMFLOAT4 dir_light_color;
	};

public:
	std::unique_ptr<Constants<DirLightParam>> light_constants{};
};

//平行光
class DirectionalLight :public Light
{
public:
	DirectionalLight(Graphics& graphics,DirectX::XMFLOAT3 dir, float r, float g, float b);
	~DirectionalLight() {}

	void debug_gui(int unique_id) override;

	void set_direction(DirectX::XMFLOAT3 position);
	void set_color(float r, float g, float b);

	DirectX::XMFLOAT3 get_direction();
	DirectX::XMFLOAT3 get_color();
};

//点光源
class PointLight :public Light
{
public:
	PointLight(Graphics& graphics, DirectX::XMFLOAT3 position, float distance, float r, float g, float b);
	~PointLight(){}

	void debug_gui(int unique_id) override;

	void set_position(DirectX::XMFLOAT3 position);
	void set_color(float r, float g, float b);
	void set_distance(float d);

	DirectX::XMFLOAT3 get_position();
	DirectX::XMFLOAT3 get_color();
	float get_distance();
};