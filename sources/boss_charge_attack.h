#pragma once
#include "aura.h"
#include "sphere.h"
#include "slash.h"
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
	//�n�ʂ���R�A�ɐL�т�x��
	std::unique_ptr<Aura> aura[6];
	//�R�A
	std::unique_ptr<Sphere> core;
	//
	std::unique_ptr<Slash> prominence[2];
};