#include "player_ui.h"
#include "user.h"
#include "imgui_include.h"
PlayerHpGauge::PlayerHpGauge(Graphics& graphics):
GaugeUi(graphics, L"./resources/Sprite/UI/Player/player_hp_bar_back.png",
	L"./resources/Sprite/UI/Player/player_hp_bar_front.png",
	nullptr)

{
	gauge.position = { 30.0f,650.0f };
	gauge.scale = { 0.15f, 0.1f };
}



PlayerUI::PlayerUI(Graphics& graphics)
{
	hp_gauge = std::make_unique <PlayerHpGauge>(graphics);
}

void PlayerUI::update(Graphics& graphics, float elapsed_time)
{
	hp_gauge->update(graphics,elapsed_time);
}

void PlayerUI::render(Graphics& graphics)
{
	hp_gauge->render(graphics.get_dc().Get());
}

void PlayerUI::set_hp_percent(float arg)
{
	hp_gauge->set_percent(arg);
}
