#pragma once
#include "aura.h"
#include "sphere.h"
class ChargeAttack : public EffecttBase
{
public:
	ChargeAttack(Graphics& grapghics);
	~ChargeAttack(){}

	void play(DirectX::XMFLOAT3 pos);
	void stop();
	void update(Graphics& graphics, float elapsed_time);
	void render(Graphics& graphics);

	std::unique_ptr<Aura> aura[6];
	std::unique_ptr<Sphere> core;
};