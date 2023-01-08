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
	//エミッターによるパーティクルの発生
	for (auto& e : emitters)
	{
		e->update(graphics, elapsed_time);
	}
}

void ParticleSystem::render(Graphics& graphics, Camera& camera)
{
	//エミッターの描画関数実行（中でパーティクルの描画処理が行われている）
	for (auto& e : emitters)
	{
		e->render(graphics,camera);
	}
}

void ParticleSystem::register_emitter(std::unique_ptr<Emitter> e)
{
	//エミッターの登録
	emitters.push_back(move(e));
}
