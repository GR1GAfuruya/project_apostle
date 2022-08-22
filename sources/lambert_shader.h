#pragma once

#include "mesh_shader.h"

class LambertShader : public MeshShader
{
public:
	LambertShader(ID3D11Device* device);
	~LambertShader()override {} ;

	static const int MAX_BONES{ 256 };

	void active(ID3D11DeviceContext* immediate_context,RenderType rt) override;
	void render(ID3D11DeviceContext* immediate_context, SkeletalMesh* model, const DirectX::XMFLOAT4X4& world);

private:
	struct BONE_CONSTANTS
	{
		DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES]{ { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } }; //最初は単位行列
	};

	std::unique_ptr<Constants<BONE_CONSTANTS>> bone_constants{};


	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> f_pixel_shader;//フォワードレンダリングの場合のPS
	Microsoft::WRL::ComPtr<ID3D11PixelShader> d_pixel_shader;//ディファ―ドレンダリングの場合のPS
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
};
