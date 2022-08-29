#pragma once
#include "graphics.h"
class SkinParticles
{
public:
	size_t max_particle_count;
	struct skin_particle
	{
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};
	struct particle
	{
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 velocity;
		float age{};
		int state{};
	};

	struct particle_constants
	{
		DirectX::XMFLOAT4 emitter_location{ 0, 0, 0, 1 };
		DirectX::XMFLOAT4 target_location{ 0, 0, 0, 1 };
		uint32_t particle_count{};
		float particle_size{ 0.01f };
		float streak_factor{ 0.05f };
		float threshold_level{ 0.0f };
	};

	particle_constants particle_data;
	size_t state{ 0 };
	uint32_t transitioned_particle_count{}; // Total number of particles that have transitioned to each state.

	Microsoft::WRL::ComPtr<ID3D11Buffer> husk_particle_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> husk_particle_buffer_uav;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> husk_particle_append_buffer_uav;

	Microsoft::WRL::ComPtr<ID3D11Buffer> updated_particle_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> updated_particle_buffer_uav;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> updated_particle_buffer_srv;

	Microsoft::WRL::ComPtr<ID3D11Buffer> completed_particle_counter_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> completed_particle_counter_buffer_uav;

	Microsoft::WRL::ComPtr<ID3D11Buffer> copied_structure_count_buffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

	SkinParticles(ID3D11Device* device, size_t max_particle_count = 500000);
	SkinParticles(const SkinParticles&) = delete;
	SkinParticles& operator=(const SkinParticles&) = delete;
	SkinParticles(SkinParticles&&) noexcept = delete;
	SkinParticles& operator=(SkinParticles&&) noexcept = delete;
	virtual ~SkinParticles() = default;

	void integrate(ID3D11DeviceContext* immediate_context, float delta_time);
	void render(ID3D11DeviceContext* immediate_context);
	void reset(ID3D11DeviceContext* immediate_context);

	Microsoft::WRL::ComPtr<ID3D11PixelShader> accumulate_husk_particles_ps;

	void accumulate_husk_particles(ID3D11DeviceContext* immediate_context, std::function<void(ID3D11PixelShader*)> drawcallback);

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> copy_buffer_cs;

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cached_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cached_depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> cached_unordered_access_view;

};