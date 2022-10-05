#pragma once
#include "aura.h"
#include "sphere.h"
#include "slash.h"
#include "wave.h"
#include "tornado.h"
#include "gpu_particle.h"
class ChargeAttack : public EffecttBase
{
public:
	ChargeAttack(Graphics& grapghics);
	~ChargeAttack(){}

	void play(DirectX::XMFLOAT3 pos);
	void stop();
	void update(Graphics& graphics, float elapsed_time);
	void render(Graphics& graphics);
	void debug_gui(const char* str_id);
	
	void set_charge_max_state() { is_charge_max = true; }
private:
	void charging_update(Graphics& graphics, float elapsed_time);
	void activities_update(Graphics& graphics, float elapsed_time);
	void vanishing_update(Graphics& graphics, float elapsed_time);

	typedef void (ChargeAttack::* ChargeAttackUpdate)(Graphics& graphics, float elapsed_time);
	ChargeAttackUpdate charge_attack_update = &ChargeAttack::charging_update;
	struct ChargeAttackConstants
	{
		DirectX::XMFLOAT3 core_pos{};
		float core_gravitation = 0.5;
		float core_radius{};
		DirectX::XMFLOAT3 pad{};
	};
	//地面からコアに伸びる支柱
	std::unique_ptr<Aura> aura[2];
	//コア
	std::unique_ptr<SphereEffect> core;

	std::unique_ptr<Wave> wave;

	std::unique_ptr<Tornado> tornado;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;

	std::unique_ptr<GPU_Particles> particle;
		
	std::unique_ptr<Constants<ChargeAttackConstants>> constants;
	const float ATTACK_TIME = 2.0f;
	bool is_charge_max = false;
	float attack_time = 0.0f;
	static constexpr DirectX::XMFLOAT4 FIRE_COLOR = { 3.0f, 0.6f, 0.0f, 0.8f };

};