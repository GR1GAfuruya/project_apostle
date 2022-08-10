#pragma once
#include "scene.h"
#include "sprite_batch.h"
#include "stage.h"
#include "camera.h"
#include "effect.h"
#include "aura.h"
#include "gpu_particle.h"
class SceneTest :public Scene
{
	struct grass_constants
	{
		float grass_height_factor{ 0.759f };
		float grass_width_factor{ 0.041f };
		float grass_curvature{ 1.100f };
		float grass_withered_factor{ 0.194f };
		float grass_height_variance{ 0.165f };
		float perlin_noise_distribution_factor{ 0.178f };
		float tesselation_max_subdivision{ 7.426f };
		float tesselation_max_distance{ 151.899f };
		float grass_specular_color[4]{ 0.885f, 0.673f, 0.328f, 1.000f };
	};
public:
	SceneTest(Graphics& graphics);
	~SceneTest()override {}

	void initialize(Graphics& graphics) override;
	void finalize() override;
	void update(float elapsed_time, Graphics& graphics) override;
	void render(float elapsed_time, Graphics& graphics) override;
	void debug_gui();
	std::unique_ptr<SkeletalMesh> model = nullptr;
	SkeletalMesh::AnimeParam dummy_animeparam{};

	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11HullShader>hull_shader;
	Microsoft::WRL::ComPtr<ID3D11DomainShader>domain_shader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views;
	std::unique_ptr<Constants<grass_constants>> grass_constant_buffer{};
	DirectX::XMFLOAT3	position = { 0, 0, 0 };
	DirectX::XMFLOAT3	angle = { 0, 0, 0 };
	DirectX::XMFLOAT3	scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4	transform = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	};

	
private:
	std::unique_ptr<Camera> camera = nullptr;
	std::unique_ptr<SpriteBatch> sprite = nullptr;
	std::unique_ptr<Stage> stage = nullptr;
	DirectX::XMFLOAT3 chara_pos{};
	std::unique_ptr<Effect> sample_effect;
	std::unique_ptr<Aura> aura = nullptr;
	std::unique_ptr<GPU_Particles> particle;

};
