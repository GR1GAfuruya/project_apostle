#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <memory>
#include <vector>
#include <string>
#include "constant.h"
#include "graphics.h"
class GPU_Particles
{
public:
	//構造体
	struct Emitter
	{
		DirectX::XMFLOAT3 pos = { 0,5,0 };
		float emit_life = 0; // エミッターの寿命
		DirectX::XMFLOAT3 velocity = { 0,0,0 };
		float emit_life_time = 0; // エミッターの年齢
		float emit_time = 0;//生成されてからの時間
		float emit_rate = 0; // 1秒間に何発発生するか
		int emit_count = 0;  // 現在の発生カウント
		float max_particle;//このエミッタが放出するパーティクルの最大数
	};

	struct ParticleData
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

	struct ParticleConstants 
	{
		Emitter emitter{};
		DirectX::XMFLOAT2 particle_size = {0.1f,0.1f};
		int particle_count;
		float particle_life_time = 1;
		DirectX::XMFLOAT3 angle{};
		float pad3;
		DirectX::XMFLOAT4 particle_color = { 0,0,0,1 };
	};
	std::unique_ptr<Constants<ParticleConstants>> particle_constants{};

	GPU_Particles(ID3D11Device* device,const int max_particle = 500000);
	GPU_Particles(const GPU_Particles&) = delete;
	GPU_Particles& operator=(const GPU_Particles&) = delete;
	GPU_Particles(GPU_Particles&&) noexcept = delete;
	GPU_Particles& operator=(GPU_Particles&&) noexcept = delete;
	~GPU_Particles();
	void initialize(Graphics& graphics);
	void launch_emitter(Microsoft::WRL::ComPtr<ID3D11ComputeShader> replace_emit_cs = nullptr);
	void particle_emit(ID3D11DeviceContext* dc);
	void update(ID3D11DeviceContext* dc, float elapsed_time, ID3D11ComputeShader* replace_update_cs = nullptr);
	void render(ID3D11DeviceContext * dc, ID3D11Device* device);
	void emitter_update(ID3D11DeviceContext* dc, float elapsed_time);
	void debug_gui(std::string str_id);
	void release_buffer();
	UINT get_particle_pool_count(ID3D11DeviceContext* dc) const;


	void set_emitter_pos(DirectX::XMFLOAT3 pos) { substitution_emitter.pos = pos; }
	void set_emitter_velocity(DirectX::XMFLOAT3 velocity) { substitution_emitter.velocity = velocity; }
	void set_emitter_life_time(float life_time);
	void set_emitter_count(int count);
	void set_emitter_rate(float rate);
	void set_particle_life_time(float life_time);
	void set_particle_size(DirectX::XMFLOAT2 size);

private:
	const int THREAD_NUM_X =16;
	size_t max_particle_count{ 0 };
	int emit_num = 0;//発生させる回数
	std::vector<std::unique_ptr<Emitter>> emitters;
	Emitter substitution_emitter;
	//実体が入るバッファー
	Microsoft::WRL::ComPtr<ID3D11Buffer> particle_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particle_buffer_uav;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particle_buffer_srv;

	//パーティクルの管理をするバッファー
	Microsoft::WRL::ComPtr<ID3D11Buffer> particle_pool_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particle_pool_buffer_uav;

	//パーティクルの数を数えるバッファー
	Microsoft::WRL::ComPtr<ID3D11Buffer> particle_count_buffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> init_cs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;
	//DebugGui
	bool display_imgui = false;
};
