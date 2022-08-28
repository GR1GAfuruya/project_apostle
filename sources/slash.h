#pragma once
#include "skeletal_mesh.h"
#include "constant.h"
#include "effect_base.h"
class Slash : public EffecttBase
{
public:
	Slash(ID3D11Device* device);
	~Slash() {};

	struct SlashConstance
	{
		DirectX::XMFLOAT2 scroll_direction;
		float scroll_speed;
		float pad;
		DirectX::XMFLOAT4 particle_color = {0,1,0,1};
	};

	void play(DirectX::XMFLOAT3 pos) {}
	void play(DirectX::XMFLOAT3 pos,DirectX::XMVECTOR slash_dir_vec, DirectX::XMVECTOR slope_vec, bool direction);
	void stop();
	void update(Graphics& graphics, float elapsed_time);
	void render(Graphics& graphics);
	
	std::unique_ptr<Constants<SlashConstance>> constance;
private:
	std::unique_ptr<MeshShader> shader;
	// スケルタルメッシュの実体
	std::unique_ptr <SkeletalMesh> model;

	
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[4];
	bool dir = false;//斬撃方向が右向きならfalse左ならtrue
	bool slash;
	float slash_timer = 0.14;
	bool display_slash_imgui = true;
};