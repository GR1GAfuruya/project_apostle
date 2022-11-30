#include "boss_ui.h"
#include "imgui_include.h"
#include "user.h"

BossUi::BossUi(Graphics& graphics) :
	GaugeUi(graphics, L"./resources/Sprite/UI/Boss/boss_hp_bar_back.png",
	L"./resources/Sprite/UI/Boss/bar.png",
	nullptr)
{
	gauge.position = { 100.0f,40.0f };
	gauge.scale = { 1.0f, 0.1f };
	gauge.color = { 1.5f,0.3f,0,1 };
	gauge.texsize = { static_cast<float>(back->get_texture2d_desc().Width), static_cast<float>(back->get_texture2d_desc().Height) };
}

void BossUi::initialize(Graphics& graphics)
{
	
}

void BossUi::update(Graphics& graphics, float elapsed_time)
{
	static bool is_display_imgui = false;
//	imgui_menu_bar("UI", "boss gauge", is_display_imgui);
//#ifdef USE_IMGUI
//	if (is_display_imgui)
//	{
//		ImGui::Begin("boss gauge");
//		ImGui::DragFloat2("pos", &gauge.position.x);
//		ImGui::DragFloat2("scale", &gauge.scale.x, 0.01f);
//		ImGui::DragFloat("hp_percent", &hp_percent, 0.1f, 0.0f, 1.0f);
//		ImGui::End();
//	}
//#endif // USE_IMGUI
}
