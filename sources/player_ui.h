#pragma once
#include "gauge_ui.h"

class PlayerHpGauge : public GaugeUi
{
public:
	PlayerHpGauge();
	~PlayerHpGauge() override {};

private:
	bool display_imgui = false;

};

class PlayerUI
{
public:
	PlayerUI();
	~PlayerUI() {};
	void update(float elapsed_time);
	void render();
	void set_hp_percent(float arg);
private:
	std::unique_ptr<PlayerHpGauge> hp_gauge;
};