#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include <vector>
#include "model_resource.h"
#include "constant.h"
struct ObjectParticle
{
	size_t max_particle_count;
	struct object_particle
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

	struct ParticleConstants
	{
		DirectX::XMFLOAT4 emitter_location{ 0, 0, 0, 1 };
		DirectX::XMFLOAT4 target_location{ 0, 0, 0, 1 };
		uint32_t particle_count{};
		float particle_size{ 0.01f };
		float streak_factor{ 0.05f };
		float threshold_level{ 0.0f };
	};

	size_t state{ 0 };
	uint32_t transitioned_particle_count{}; // Total number of particles that have transitioned to each state.

	Microsoft::WRL::ComPtr<ID3D11Buffer> object_particle_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> object_particle_buffer_uav;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> object_particle_append_buffer_uav;

	Microsoft::WRL::ComPtr<ID3D11Buffer> updated_particle_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> updated_particle_buffer_uav;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> updated_particle_buffer_srv;

	Microsoft::WRL::ComPtr<ID3D11Buffer> completed_particle_counter_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> completed_particle_counter_buffer_uav;

	Microsoft::WRL::ComPtr<ID3D11Buffer> copied_structure_count_buffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader;
	std::unique_ptr<Constants<ParticleConstants>> constant;

	ObjectParticle(ID3D11Device* pDevice, size_t max_particle_count = 500000);
	ObjectParticle(const ObjectParticle&) = delete;
	ObjectParticle& operator=(const ObjectParticle&) = delete;
	ObjectParticle(ObjectParticle&&) noexcept = delete;
	ObjectParticle& operator=(ObjectParticle&&) noexcept = delete;
	virtual ~ObjectParticle() = default;

	void integrate(ID3D11DeviceContext* immediate_context, float delta_time);
	void render(ID3D11DeviceContext* immediate_context);
	void reset(ID3D11DeviceContext* immediate_context);

	Microsoft::WRL::ComPtr<ID3D11PixelShader> accumulate_object_particles_ps;
	void accumulate_object_particles(ID3D11DeviceContext* immediate_context, std::function<void(ID3D11PixelShader*)> drawcallback);

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> copy_buffer_cs;

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cached_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cached_depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> cached_unordered_access_view;

};