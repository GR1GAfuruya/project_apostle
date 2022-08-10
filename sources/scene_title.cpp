#include "scene_title.h"
#include "scene_loading.h"
#include "scene_game.h"
#include "scene_manager.h"
#include "device.h"

SceneTitle::SceneTitle(Graphics& graphics)
{
	sprite = std::make_unique<SpriteBatch>(graphics.get_device().Get(),L".\\resources\\Sprite\\title.jpg",1);
}

void SceneTitle::initialize(Graphics& graphics)
{
}

void SceneTitle::finalize()
{
}

void SceneTitle::update(float elapsedTime, Graphics& graphics)
{
	Mouse& mouse = Device::instance().get_mouse();
	if (mouse.get_button() & mouse.BTN_Z)
	{
		SceneManager::instance().change_scene(graphics,new SceneLoading(new SceneGame(graphics)));
	}
}

void SceneTitle::render(float elapsedTime,Graphics& graphics)
{
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_OFF_ZW_OFF, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	sprite->begin(graphics.get_dc().Get());
	sprite->render(graphics.get_dc().Get(), { 0, 0 }, { 1, 1 });
	sprite->end(graphics.get_dc().Get());
}
