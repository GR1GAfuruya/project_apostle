#pragma once
#include "graphics.h"
#include "constant.h"
class Light
{
public:
	Light(Graphics& graphics);
	~Light(){}

	//-------<�֐�>-------//
	virtual void debug_gui(int unique_id){};
protected:
	//�萔�o�b�t�@
	struct DirLightParam {
		DirectX::XMFLOAT4 dir_light_dir;
		DirectX::XMFLOAT4 dir_light_color;
	};

public:
	std::unique_ptr<Constants<DirLightParam>> light_constants{};
};

//���s��
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

//�_����
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