#pragma once
#include "particle.h"
#include "emitter.h"
class ParticleSystem
{
public:
	ParticleSystem(Graphics& graphics);
	virtual ~ParticleSystem();

	void Play(DirectX::XMFLOAT3 pos);


	void update(Graphics& graphics, float elapsed_time);

	void render(Graphics& graphics, Camera& camera);

	void register_emitter(std::unique_ptr<Emitter> p);
private:

	std::vector<std::unique_ptr<Emitter>> emitters;

	//パーティクル全体の期間
	float duration;

	float time;

	DirectX::XMFLOAT3	position = { 0, 0, 0 };
	DirectX::XMFLOAT3	scale = { 1, 1, 1 };
	DirectX::XMFLOAT4 orientation{ 0,0,0,1 };
	DirectX::XMFLOAT4X4	transform = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	};
};