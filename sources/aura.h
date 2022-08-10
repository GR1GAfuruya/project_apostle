#pragma once
#include <DirectXMath.h>

#include "constant.h"
#include "graphics.h"
#include "shader.h"

class Aura
{
public:
	Aura(Graphics& graphics);
	~Aura(){}
	void update(Graphics& graphics,float elapsed_time);
public:
	struct AURA_CONSTANTS
	{
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 move_vec;
		float time;
		float pad;
	};
	std::unique_ptr<Constants<AURA_CONSTANTS>> aura_constants{};
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;

};
