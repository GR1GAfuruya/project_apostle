#pragma once
#include <WICTextureLoader.h>
#include <wrl.h>
#include "misc.h"
#include <memory>
#include <DirectXMath.h>

#include "shader.h"
class GeometricPrimitive
{
public:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};
	struct constans
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

public:
	GeometricPrimitive(ID3D11Device* device);
	virtual ~GeometricPrimitive() = default;

	void render(ID3D11DeviceContext* immediate_context,
		const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color);

protected:
	void create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
		uint32_t* indices, size_t index_count);

};

