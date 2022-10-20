#include "scene_title.h"
#include "scene_loading.h"
#include "scene_game.h"
#include "scene_manager.h"
#include "device.h"

SceneTitle::SceneTitle(Graphics& graphics)
{
	sprite = std::make_unique<SpriteBatch>(graphics.get_device().Get(),L".\\resources\\Sprite\\title.png",1);
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
	GamePad& game_pad = Device::instance().get_game_pad();
	if (mouse.get_button() & mouse.BTN_Z || game_pad.get_button() & game_pad.BTN_A)
	{
		SceneManager::instance().change_scene(graphics,new SceneLoading(new SceneGame(graphics)));
	}
}

void SceneTitle::render(float elapsedTime,Graphics& graphics)
{
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_OFF_ZW_OFF, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	sprite->begin(graphics.get_dc().Get());
	sprite->render(graphics.get_dc().Get(), { 0, 0 }, { MAGNI_RESOLUTION_WIDTH, MAGNI_RESOLUTION_HEIGHT });
	sprite->end(graphics.get_dc().Get());
}
