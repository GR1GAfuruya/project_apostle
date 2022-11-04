#include "player_ui.h"
#include "user.h"
#include "imgui_include.h"
PlayerHpGauge::PlayerHpGauge(Graphics& graphics)
{
	back = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L"./resources/Sprite/UI/Player/player_hp_bar_back.png", 1);
	body = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L"./resources/Sprite/UI/Player/player_hp_bar_front.png", 1);

	gauge.position = { 30.0f,650.0f };
	gauge.scale = { 0.15f, 0.1f };
	gauge.texsize = { static_cast<float>(back->get_texture2d_desc().Width), static_cast<float>(back->get_texture2d_desc().Height) };
}

void PlayerHpGauge::update(Graphics& graphics, float elapsed_time)
{
//	imgui_menu_bar("UI", "player_hp", display_imgui);
//#ifdef USE_IMGUI
//	if (display_imgui)
//	{
//		ImGui::Begin("player gauge");
//		ImGui::DragFloat2("pos", &gauge.position.x);
//		ImGui::DragFloat2("scale", &gauge.scale.x, 0.01f);
//		ImGui::DragFloat("hp_percent", &hp_percent, 0.1f, 0.0f, 1.0f);
//		ImGui::End();
//	}
//
//#endif // USE_IMGUI}

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
	hp_gauge->set_hp_percent(arg);
}
