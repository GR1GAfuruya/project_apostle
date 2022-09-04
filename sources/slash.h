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
		DirectX::XMFLOAT4 particle_color = { 0,0,0,1 };
		DirectX::XMFLOAT2 scroll_direction;
		float scroll_speed;
		float thirethold;
	};

	void play(DirectX::XMFLOAT3 pos)override;
	void play(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 slash_dir_vec, float tilt_degree, bool direction);
	void stop()override;
	void update(Graphics& graphics, float elapsed_time)override;
	void render(Graphics& graphics)override;
	void debug_gui(const char* str_id);

	void set_rotate_rate(float rate) { rotate_rate = rate; }
	std::unique_ptr<Constants<SlashConstance>> constants;
private:
	std::unique_ptr<MeshShader> shader;
	// スケルタルメッシュの実体
	std::unique_ptr <SkeletalMesh> model;

	
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[4];
	bool dir = false;//斬撃方向が右向きならfalse左ならtrue
	float rotate_rate = 20;
	bool display_slash_imgui = true;
};