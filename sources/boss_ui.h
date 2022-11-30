#pragma once
#include "gauge_ui.h"

class BossUi :public GaugeUi
{
public:
	BossUi(Graphics& graphics);
	~BossUi() override{};

	void initialize(Graphics& graphics);
	void update(Graphics& graphics, float elapsed_time) override;
private:
};
