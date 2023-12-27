#include "effect.h"
#include "effect_manager.h"
#include "sprite_emitter.h"
#include "particle_system.h"
Effect::Effect(const char* file_name)
{
	//effect = EffectManager::instance().create();
	psrticle_system = EffectManager::instance().create();
	psrticle_system->add_component<ParticleSystem>(file_name);
}

Effect::~Effect()
{
}

void Effect::play(DirectX::XMFLOAT3 pos)
{
	psrticle_system->get_component<ParticleSystem>()->Launch(pos);

}

void Effect::stop()
{
	EffectManager::instance().remove(psrticle_system);
}

void Effect::set_position(DirectX::XMFLOAT3 pos)
{
	//effect->get_component<Transform>().get()->set_position(pos);
}

void Effect::debug_gui(string id)
{
	psrticle_system->get_component<ParticleSystem>().get()->on_gui();
}
