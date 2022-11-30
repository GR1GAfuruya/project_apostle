#pragma once
#include "gauge_ui.h"

class PlayerHpGauge : public GaugeUi
{
public:
	PlayerHpGauge(Graphics& graphics);
	~PlayerHpGauge() override {};

private:
	bool display_imgui = false;

};

class PlayerUI
{
public:
	PlayerUI(Graphics& graphics);
	~PlayerUI() {};
	void update(Graphics& graphics, float elapsed_time);
	void render(Graphics& graphics);
	void set_hp_percent(float arg);
private:
	std::unique_ptr<PlayerHpGauge> hp_gauge;
};