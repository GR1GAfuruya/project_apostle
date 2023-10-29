#include "effect.h"
#include "effect_manager.h"
#include "transform.h"
#include "sprite_emitter.h"
Effect::Effect(const char* file_name)
{
	//effect = EffectManager::instance().create();
	//effect->add_component<Transform>();
}

Effect::~Effect()
{
}

void Effect::play(DirectX::XMFLOAT3 pos)
{
	//すでに再生されてたら消す
	if (effect)
	{
		EffectManager::instance().remove(effect);
	}
	effect = EffectManager::instance().create();
	effect->add_component<Transform>();
	effect->add_component<SpriteEmitter>(100/*事前に保存しておいた最大パーティクル数を入力*/);
	//effect->add_component<Material>();
	effect->set_name("ファイルから読みだしたエミッター名" /*+ seed*/);

}

void Effect::stop()
{
	if(effect != nullptr)
	EffectManager::instance().remove(effect);
}

void Effect::set_position(DirectX::XMFLOAT3 pos)
{
	effect->get_component<Transform>().get()->set_position(pos);
}

void Effect::debug_gui(string id)
{
	if (effect)
	effect->get_component<SpriteEmitter>().get()->debug_gui(id);
}
