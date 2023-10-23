#pragma once
#include "graphics.h"
#include "constant.h"
class Light
{
public:
	Light();
	~Light() {}

	//-------<�֐�>-------//
	virtual void debug_gui(std::string name) {};
protected:
	//�萔�o�b�t�@
	struct DirLightParam {
		DirectX::XMFLOAT4 dir_light_dir;
		DirectX::XMFLOAT4 dir_light_color;
	};

public:
	std::unique_ptr<Constants<DirLightParam>> light_constants{};
	std::string name;
};

//���s��
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

//�_����
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