#include "particle_system.h"
#include "effect_manager.h"
#include "sprite_emitter.h"
ParticleSystem::ParticleSystem(const char* file_path)
{
	shared_ptr<GameObject> effect = EffectManager::instance().create();
	effect->transform.set_position(position);
	SpriteEmitter::Param param;
	effect->add_component<SpriteEmitter>(param,100/*事前に保存しておいた最大パーティクル数を入力*/);
	//エミッターの登録
	emitters.push_back(move(effect));

}

ParticleSystem::~ParticleSystem()
{
	emitters.clear();
}

void ParticleSystem::Launch(DirectX::XMFLOAT3 pos)
{
	position = pos;
	active = true;

}

void ParticleSystem::init()
{
	time = 0;
	active = false;

}

void ParticleSystem::update(float elapsed_time)
{
	if (active)
	{
		time += elapsed_time;

		//エミッターによるパーティクルの発生
		for (auto& e : emitters)
		{
			if (time > e->get_component<SpriteEmitter>()->param.emit_start_time)
			{
				e->get_component<SpriteEmitter>()->play(position);
			}
		}
	}
}


void ParticleSystem::on_gui()
{
}

void ParticleSystem::register_emitter(std::shared_ptr<GameObject> e)
{
	//エミッターの登録
	emitters.push_back(move(e));
}
