#include "Effect.h"
#include "EffectManager.h"
#include "transform.h"
Effect::Effect(const char* file_name)
{
	//EffectManager::instance().create()
}

Effect::~Effect()
{
}

void Effect::play(DirectX::XMFLOAT3 pos)
{
	effect = EffectManager::instance().create();
	effect->set_name("ファイルから読みだしたエミッター名" /*+ seed*/);
	effect->add_component<Transform>();
}

void Effect::stop()
{
	EffectManager::instance().remove(effect);
}

void Effect::set_position(DirectX::XMFLOAT3 pos)
{
	effect->get_component<Transform>().get()->set_position(pos);
}
