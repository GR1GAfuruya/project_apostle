#pragma once
#include "particle.h"
#include "emitter.h"
class ParticleSystem
{
public:
	ParticleSystem(Graphics& graphics);
	virtual ~ParticleSystem();

	void update(Graphics& graphics, float elapsed_time);

	void render(Graphics& graphics, Camera& camera);

	void register_emitter(std::unique_ptr<Emitter> p);
private:

	std::vector<std::unique_ptr<Emitter>> emitters;

	//�p�[�e�B�N���S�̂̊���
	float duration;

	float time;
};