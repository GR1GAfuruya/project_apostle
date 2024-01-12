#include "effect.h"
#include "effect_manager.h"
#include "sprite_emitter.h"
#include "particle_system.h"
Effect::Effect(const char* file_name)
{
	//effect = EffectManager::instance().create();
	particle_system = EffectManager::instance().create();
	particle_system->add_component<ParticleSystem>(file_name);
	particle_system->set_name("effec");
}

Effect::~Effect()
{
}

void Effect::play(DirectX::XMFLOAT3 pos)
{
	particle_system->get_component<ParticleSystem>()->init();
	particle_system->get_component<ParticleSystem>()->Launch(pos);

}

void Effect::stop()
{
	EffectManager::instance().remove(particle_system);
}

void Effect::set_position(DirectX::XMFLOAT3 pos)
{
	particle_system->transform.set_position(pos);
}

void Effect::set_scale(DirectX::XMFLOAT3 scale)
{
	particle_system->transform.set_scale(scale);
}

void Effect::debug_gui(string id)
{
	particle_system->get_component<ParticleSystem>().get()->on_gui();
}
