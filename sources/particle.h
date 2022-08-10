#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <memory>
#include <vector>
#include "constant.h"
#include "graphics.h"
#include "sprite_batch.h"
class Particles
{
private:
	//�\����
	struct Emitter
	{
		DirectX::XMFLOAT3 position;
		float emit_life; // �G�~�b�^�[�̎���
		float emit_life_time; // �G�~�b�^�[�̔N��
		float emit_time;//��������Ă���̎���
		float emit_rate; // 1�b�Ԃɉ����������邩
		int emit_count;  // ���݂̔����J�E���g
		DirectX::XMFLOAT3 force;
	};
	Emitter emitter;

	struct  Particle
	{
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT3 rotate;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 velocity;
		DirectX::XMFLOAT3 force;
		bool is_active;
		float time;
		float life_time;
	};

	Particle* data;

	struct Vertex {
		DirectX::XMFLOAT3 position;		// ���W
		//DirectX::XMFLOAT3 normal; //�@��
		DirectX::XMFLOAT2 uv;		// UV
		DirectX::XMFLOAT4 color;
	};
	std::vector<Vertex> vertices;

	//�ϐ�
	static const int max_particles = 30000;
	int active_num;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;		
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;		
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;

	D3D11_TEXTURE2D_DESC texture2d_desc;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;


public:
	Particles(Graphics& graphics);
	Particles(const Particles&) = delete;
	Particles& operator=(const Particles&) = delete;
	Particles(Particles&&) noexcept = delete;
	Particles& operator=(Particles&&) noexcept = delete;
	~Particles();

	void initialize(Graphics& graphics);
	void update(Graphics& graphics, float elapsed_time);
	void render(Graphics& graphics);
	void emit();
	std::unique_ptr<SpriteBatch> sprite = nullptr;

	void set_emit_param(float life, float rate, DirectX::XMFLOAT2 emitpos);

};