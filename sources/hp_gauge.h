#pragma once
#include "UI.h"
class HpGauge : public UI
{

public:
	//--------<constructor/destructor>--------//
	HpGauge()
	:hp_percent(0)
	{}
	virtual ~HpGauge() {}

	//--------< ŠÖ” >--------//
	void update(Graphics& graphics, float elapsed_time) override;
	void render(ID3D11DeviceContext* dc) override;

	void set_hp_percent(float arg) { hp_percent = arg; }
	void set_animation(bool arg) { animation = arg; }
protected:
	//--------< •Ï” >--------//
	std::unique_ptr<SpriteBatch> frame{ nullptr };
	std::unique_ptr<SpriteBatch> back{ nullptr };
	std::unique_ptr<SpriteBatch> body{ nullptr };

	Element gauge;
	float hp_percent;
	bool animation = false;
};