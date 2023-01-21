#pragma once
#include "UI.h"
class GaugeUi : public UI
{

public:
	//--------<constructor/destructor>--------//
	GaugeUi() :now_percent(1)
	{}
	GaugeUi(Graphics& graphics, const wchar_t* back_filename, const wchar_t* body_filename, const wchar_t* frame_filename);
	virtual ~GaugeUi() {}

	//--------< ŠÖ” >--------//
	void update(Graphics& graphics, float elapsed_time) override;
	void render(ID3D11DeviceContext* dc) override;

	void set_percent(float per) { now_percent = per; }
	void set_animation(bool anim) { animation = anim; }
	void set_angle(float ang) { gauge.angle = ang; }
	void set_position(DirectX::XMFLOAT2 pos) { gauge.position = pos; }
	void set_scale(DirectX::XMFLOAT2 scale) { gauge.scale = scale; }
	void set_tex_size(DirectX::XMFLOAT2 texsize) { gauge.texsize = texsize; }
	void set_color(DirectX::XMFLOAT4 color) { gauge.color = color; }
	void set_diff_color(DirectX::XMFLOAT4 color) { diff_color = color; }
protected:
	//--------< •Ï” >--------//
	std::unique_ptr<SpriteBatch> frame{ nullptr };
	std::unique_ptr<SpriteBatch> back{ nullptr };
	std::unique_ptr<SpriteBatch> body{ nullptr };
	DirectX::XMFLOAT4 diff_color = { 1.0f,1.0f,1.0f,1.0f };
	Element gauge;
	float now_percent;
	float old_percent;
	bool animation = false;
};