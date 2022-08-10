#pragma once
#include "graphics.h"
class SkinParticles
{
public:
	SkinParticles(ID3D11Device* device, size_t max_particle_count = 500000);
	SkinParticles(const SkinParticles&) = delete;
	SkinParticles& operator=(const SkinParticles&) = delete;
	SkinParticles(SkinParticles&&) noexcept = delete;
	SkinParticles& operator=(SkinParticles&&) noexcept = delete;
	virtual ~SkinParticles() = default;


	Microsoft::WRL::ComPtr<ID3D11Buffer> husk_particle_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> husk_particle_buffer_uav;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> husk_particle_append_buffer_uav;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> compute_shader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
};