#pragma once
#include "hp_gauge.h"

class BossUi :public HpGauge
{
public:
	BossUi(Graphics& graphics);
	~BossUi() override{};

	void initialize(Graphics& graphics);
	void update(Graphics& graphics, float elapsed_time) override;
private:
};
