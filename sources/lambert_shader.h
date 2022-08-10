#pragma once

#include "shader.h"

class LambertShader : public Shader
{
public:
	LambertShader(ID3D11Device* device);
	~LambertShader()override {} ;

	static const int MAX_BONES{ 256 };

	void active(ID3D11DeviceContext* immediate_context,RenderType rt) override;
	void render(ID3D11DeviceContext* immediate_context, SkeletalMesh* model, const DirectX::XMFLOAT4X4& world);

private:

	struct OBJECT_CONSTANTS
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
	};

	struct BONE_CONSTANTS
	{
		DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES]{ { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } }; //最初は単位行列
	};

	struct MATERIAL_CONSTANTS
	{
		DirectX::XMFLOAT2 texcoord_offset{ 0.0f, 0.0f };
		DirectX::XMFLOAT2 texcoord_scale{ 1.0f, 1.0f };
		float emissive_power = 1.0f;
		DirectX::XMFLOAT3 pad;
	};
	struct shader_resources
	{
		MATERIAL_CONSTANTS material_data;
		ID3D11ShaderResourceView* shader_resource_views[4];
	};

	std::unique_ptr<Constants<OBJECT_CONSTANTS>> object_constants{};
	std::unique_ptr<Constants<BONE_CONSTANTS>> bone_constants{};
	std::unique_ptr<Constants<MATERIAL_CONSTANTS>> material_constants{};

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> f_pixel_shader;//フォワードレンダリングの場合のPS
	Microsoft::WRL::ComPtr<ID3D11PixelShader> d_pixel_shader;//ディファ―ドレンダリングの場合のPS
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
};
