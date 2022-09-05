#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>
#include "constant.h"
#include <memory>
class field_spark_particles
{
public:
	struct spark_particle
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 velocity;
		DirectX::XMFLOAT4 color;
		float time;
		float streak_factor;
	};
	std::vector<spark_particle> particles;

	struct PARTICLE_CONSTANTS
	{
		DirectX::XMFLOAT3 current_eye_position;
		float pad;
		DirectX::XMFLOAT3 previous_eye_position;
		float pad2;

		// Radius of outermost orbit 
		float outermost_radius;
		// Height of spark area
		float spark_area_height;

		float particle_size;
		uint32_t particle_count;
	};
	std::unique_ptr<Constants<PARTICLE_CONSTANTS>> particle_constants{};

	field_spark_particles(ID3D11Device* device, DirectX::XMFLOAT3 initial_position);
	field_spark_particles(const field_spark_particles&) = delete;
	field_spark_particles& operator=(const field_spark_particles&) = delete;
	field_spark_particles(field_spark_particles&&) noexcept = delete;
	field_spark_particles& operator=(field_spark_particles&&) noexcept = delete;
	virtual ~field_spark_particles() = default;

	void update(ID3D11DeviceContext* dc, float delta_time, DirectX::XMFLOAT3 eye_position);
	void render(ID3D11DeviceContext* dc);

private:
	size_t spark_particle_count{ 0 };
	Microsoft::WRL::ComPtr<ID3D11Buffer> spark_particle_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> spark_particle_buffer_uav;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> spark_particle_buffer_srv;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

	bool display_imgui = false;
};