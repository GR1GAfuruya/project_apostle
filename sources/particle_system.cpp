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
	//エミッターによるパーティクルの発生
	for (auto& e : emitters)
	{
		e->update(elapsed_time);
	}
}

void ParticleSystem::render(Camera& camera)
{
	//エミッターの描画関数実行（中でパーティクルの描画処理が行われている）
	for (auto& e : emitters)
	{
		e->render(camera);
	}
}

void ParticleSystem::register_emitter(std::unique_ptr<Emitter> e)
{
	//エミッターの登録
	emitters.push_back(move(e));
}
