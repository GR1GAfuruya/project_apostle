#pragma once
#include "gauge_ui.h"

class BossUi :public GaugeUi
{
public:
	BossUi();
	~BossUi() override{};

	void initialize();
	void update(float elapsed_time) override;
private:
};
