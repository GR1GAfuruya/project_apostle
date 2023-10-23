#include "particle_system.h"

ParticleSystem::ParticleSystem()
{

}

ParticleSystem::~ParticleSystem()
{
	emitters.clear();
}

void ParticleSystem::Play(DirectX::XMFLOAT3 pos)
{
	position = pos;
}

void ParticleSystem::update(float elapsed_time)
{
	//�G�~�b�^�[�ɂ��p�[�e�B�N���̔���
	for (auto& e : emitters)
	{
		e->update(elapsed_time);
	}
}

void ParticleSystem::render(Camera& camera)
{
	//�G�~�b�^�[�̕`��֐����s�i���Ńp�[�e�B�N���̕`�揈�����s���Ă���j
	for (auto& e : emitters)
	{
		e->render(camera);
	}
}

void ParticleSystem::register_emitter(std::unique_ptr<Emitter> e)
{
	//�G�~�b�^�[�̓o�^
	emitters.push_back(move(e));
}
