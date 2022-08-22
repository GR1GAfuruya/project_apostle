#pragma once

#include "mesh_shader.h"

class PBRShader : public MeshShader
{
public:
	PBRShader(ID3D11Device* device);
	~PBRShader()override {};

	static const int MAX_BONES{ 256 };

	void active(ID3D11DeviceContext* immediate_context, RenderType rt) override;
	void render(ID3D11DeviceContext* immediate_context, SkeletalMesh* model, const DirectX::XMFLOAT4X4& world);

	protected:

	struct BONE_CONSTANTS
	{
		DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES]{ { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } }; //最初は単位行列
	};

	struct shader_resources
	{
		MATERIAL_CONSTANTS material_data;
		ID3D11ShaderResourceView* shader_resource_views[8];
	};

	std::unique_ptr<Constants<BONE_CONSTANTS>> bone_constants{};

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> f_pixel_shader;//フォワードレンダリングの場合のPS
	Microsoft::WRL::ComPtr<ID3D11PixelShader> d_pixel_shader;//ディファ―ドレンダリングの場合のPS
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
};
