#pragma once
#include "skeletal_mesh.h"
#include "constant.h"
#include "shader.h"
#include "graphics.h"
#include "mesh_shader.h"
class Slash
{
public:
	Slash(ID3D11Device* device);
	~Slash() {};

	struct SlashConstance
	{
		DirectX::XMFLOAT2 scroll_direction;
		float scroll_speed;
		float pad;
	};
	void update(float elapsed_time);
	void render(Graphics& graphics);
	void launch(DirectX::XMFLOAT3 pos,DirectX::XMVECTOR slash_dir_vec, DirectX::XMVECTOR slope_vec);
	//位置設定
	void set_position(const DirectX::XMFLOAT3& position) { this->position = position; }


	std::unique_ptr<Constants<SlashConstance>> constance;
	std::unique_ptr<MeshShader> shader;
	// スケルタルメッシュの実体
	std::unique_ptr <SkeletalMesh> model;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
	DirectX::XMFLOAT3 scale;

	DirectX::XMFLOAT3 end_slash_vec;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	bool slash;
	float slash_timer;
	bool display_slash_imgui = true;
};