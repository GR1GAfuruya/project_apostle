#pragma once
#include "aura.h"
#include "sphere.h"
#include "slash.h"
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

private:
	//地面からコアに伸びる支柱
	std::unique_ptr<Aura> aura[6];
	//コア
	std::unique_ptr<Sphere> core;
	//
	std::unique_ptr<Slash> prominence[2];

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;

	std::unique_ptr<GPU_Particles> particle;

};