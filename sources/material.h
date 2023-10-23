#pragma once
#include "graphics.h"

class Material
{
public:
	Material();
	~Material() {}

	//ID取得
	string get_id() { return id; }
	//SRVをGPUに転送
	void transfer_shader_resource_view();
	//SRVをリストに登録
	void register_shader_resource(ID3D11Device* device, const wchar_t* filename);

	void create_pixel_shader(ID3D11Device* device, const char* cso_name);

	void reload();

	const Microsoft::WRL::ComPtr<ID3D11VertexShader> get_vs() { return vertex_shader.Get(); }
	const Microsoft::WRL::ComPtr<ID3D11PixelShader> get_ps() { return pixel_shader.Get(); }

protected:
	//頂点シェーダー
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	//ピクセルシェーダー
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	//シェーダーリソースビューのコンテナ
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> shader_resources;
	//HLSL上でのレジスターのスタート位置
	const int MATERIAL_START_SLOT = 20;
	//マテリアルID
	string id;
};