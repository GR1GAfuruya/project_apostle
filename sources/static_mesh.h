#pragma once
#include <WICTextureLoader.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <fstream>
#include <filesystem>
#include <string>

#include "constant.h"
#include "shader.h"
#include "texture.h"
#include "geometric_primitive.h"
class StaticMesh
{
public:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};
	struct OBJECT_CONSTANTS
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 Ka;
		DirectX::XMFLOAT4 Kd;
		DirectX::XMFLOAT4 Ks;
	};

	struct subset
	{
		std::wstring usemtl; //マテリアル名
		uint32_t index_start{ 0 }; // インデックスの開始位置
		uint32_t index_count{ 0 }; //インデックスの数（頂点数）
	};
	std::vector<subset> subsets;

	//std::wstring texture_filename;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	struct material
	{
		std::wstring name;
		DirectX::XMFLOAT4 Ka{ 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f, 0.8f, 0.8f, 1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f, 1.0f, 1.0f, 1.0f };
		std::wstring texture_filenames[2];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[2];
	};
	DirectX::XMFLOAT3 bounding_box[2]{ { D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX }, { -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX } };
	std::vector<material>materials;


private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	std::unique_ptr<Constants<OBJECT_CONSTANTS>> object_constants;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ramp_texture;

	
public:
	StaticMesh(ID3D11Device* device, const wchar_t* obj_filename, bool flipping_v_coordinates);
	virtual ~StaticMesh() = default;

	void render(ID3D11DeviceContext* immediate_context,
		const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color, ID3D11PixelShader* bound_pixel_shader = nullptr);

protected:
	void create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
		uint32_t* indices, size_t index_count);

	
};
