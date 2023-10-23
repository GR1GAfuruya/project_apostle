#include "player_ui.h"
#include "user.h"
#include "imgui_include.h"
PlayerHpGauge::PlayerHpGauge() :
	GaugeUi(L"./resources/Sprite/UI/Player/player_hp_bar_back.png",
		L"./resources/Sprite/UI/Player/bar.png",
		nullptr)
{
	gauge.position = { 30.0f,650.0f };
	gauge.scale = { 0.15f, 0.1f };
	gauge.color = { 0.5f,1,0,1 };
	diff_color = { 1.0f,0.0f, 0.0f, 1.0f };
}



PlayerUI::PlayerUI()
{
	hp_gauge = std::make_unique <PlayerHpGauge>();
}

void PlayerUI::update(float elapsed_time)
{
	hp_gauge->update(elapsed_time);
}

void PlayerUI::render()
{
	Graphics& graphics = Graphics::instance();
	hp_gauge->render(graphics.get_dc().Get());
}

void PlayerUI::set_hp_percent(float arg)
{
	hp_gauge->set_percent(arg);
}
