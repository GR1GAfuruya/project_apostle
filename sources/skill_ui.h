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

	SkillUI(Graphics& graphics, const wchar_t* filename);
	~SkillUI(){}
	void initialize(SlotsUi init_param, int slots_num);
	void update(Graphics& graphics,float elapsed_time);
	void icon_render(Graphics& graphics);
	void selected_skill_icon_render(Graphics& graphics, DirectX::XMFLOAT2 pos);
	void debug_gui(string str_id);
	void set_skill_select(bool s) { skill_select = s; };
	void set_selected_skill_index(int i) { selected_skill_index = i; };
private:
	std::unique_ptr<SpriteBatch> skill_slot_icon;
	
	SlotsUi slots_ui;
	bool skill_select;
	int selected_skill_index;
	int slots_num;

	float expansion_start = 30;
	float expansion_end = 150;
	float add_alpha_speed = 5;

	float add_ang_start = -120;
	float add_ang_end = -90;
	float add_ang_lerp_speed = 6;

	bool display_imgui = false;
};