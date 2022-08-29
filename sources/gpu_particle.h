#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <memory>
#include "constant.h"
class GPU_Particles
{
public:
	//�\����
	struct Emitter
	{
		DirectX::XMFLOAT3 pos = { 0,5,0 };

		float emit_life = 0; // �G�~�b�^�[�̎���
		float emit_life_time = 0; // �G�~�b�^�[�̔N��
		float emit_time = 0;//��������Ă���̎���
		float emit_rate = 0; // 1�b�Ԃɉ����������邩
		int emit_count = 0;  // ���݂̔����J�E���g
	};

	struct PARTICLE_DATA
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 velocity;
		DirectX::XMFLOAT3 angle;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 scale = {1,1,1};
		float streak_factor;
		float time;
		float life_time;
		bool is_active;
	};

	struct PARTICLE_CONSTANTS 
	{
		Emitter emitter{};
		DirectX::XMFLOAT2 particle_size = {0.1,0.1};
		int particle_count;
		float pad2;
		DirectX::XMFLOAT3 angle{};
		float pad3;
		DirectX::XMFLOAT4 particle_color = { 0,0,0,1.0 };
	};
	std::unique_ptr<Constants<PARTICLE_CONSTANTS>> particle_constants{};

	GPU_Particles(ID3D11Device* device,const int max_particle = 500000);
	GPU_Particles(const GPU_Particles&) = delete;
	GPU_Particles& operator=(const GPU_Particles&) = delete;
	GPU_Particles(GPU_Particles&&) noexcept = delete;
	GPU_Particles& operator=(GPU_Particles&&) noexcept = delete;
	virtual ~GPU_Particles() = default;

	void initialize(ID3D11DeviceContext* dc);
	void launch_emitter(ID3D11DeviceContext* dc,float emit_life, Microsoft::WRL::ComPtr<ID3D11ComputeShader> replace_emit_cs = nullptr);
	void particle_emit(ID3D11DeviceContext* dc);
	void update(ID3D11DeviceContext* dc, float elapsed_time, ID3D11ComputeShader* replace_update_cs = nullptr);
	void render(ID3D11DeviceContext * dc, ID3D11Device* device);
	UINT get_particle_pool_count(ID3D11DeviceContext* dc) const;


	void set_emit_pos(DirectX::XMFLOAT3 pos) { particle_constants->data.emitter.pos = pos; }

private:
	const int THREAD_NUM_X =16;
	size_t particle_count{ 0 };
	//���̂�����o�b�t�@�[
	Microsoft::WRL::ComPtr<ID3D11Buffer> particle_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particle_buffer_uav;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particle_buffer_srv;

	//�p�[�e�B�N���̊Ǘ�������o�b�t�@�[
	Microsoft::WRL::ComPtr<ID3D11Buffer> particle_pool_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particle_pool_buffer_uav;

	//�p�[�e�B�N���̐��𐔂���o�b�t�@�[
	Microsoft::WRL::ComPtr<ID3D11Buffer> particle_count_buffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> init_cs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
};
