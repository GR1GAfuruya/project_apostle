#include "particle_system.h"

ParticleSystem::ParticleSystem(Graphics& graphics)
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

void ParticleSystem::update(Graphics& graphics, float elapsed_time)
{
	//�G�~�b�^�[�ɂ��p�[�e�B�N���̔���
	for (auto& e : emitters)
	{
		e->update(graphics, elapsed_time);
	}
}

void ParticleSystem::render(Graphics& graphics, Camera& camera)
{
	//�G�~�b�^�[�̕`��֐����s�i���Ńp�[�e�B�N���̕`�揈�����s���Ă���j
	for (auto& e : emitters)
	{
		e->render(graphics,camera);
	}
}

void ParticleSystem::register_emitter(std::unique_ptr<Emitter> e)
{
	//�G�~�b�^�[�̓o�^
	emitters.push_back(move(e));
}
