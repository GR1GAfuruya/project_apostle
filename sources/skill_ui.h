#pragma once
#include "graphics.h"
#include "sprite_batch.h"
class SkillUI
{
public:
	struct SlotsUi
	{
		DirectX::XMFLOAT2 center_pos;
		float radius;
		float add_ang;
		DirectX::XMFLOAT2 icon_pos;
		DirectX::XMFLOAT2 texsize;
		float size;
		float expansion;
		float expansion_speed;
		DirectX::XMFLOAT4 color;

	};

	SkillUI(Graphics& graphics);
	~SkillUI(){}
	void initialize(SlotsUi init_param);
	void update(Graphics& graphics,float elapsed_time);
	void icon_render(Graphics& graphics);
	void set_skill_select(bool s) { skill_select = s; };
private:
	std::unique_ptr<SpriteBatch> skill_slot_icon;
	
	SlotsUi slots_ui;
	bool skill_select;

	int slots_num;
};