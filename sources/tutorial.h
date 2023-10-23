#pragma once
#include "UI.h"
#include "sprite_batch.h"
#include "device.h"
#include "imgui_include.h"
#include "user.h"
class Tutorial :public UI
{
public:
	Tutorial();
	~Tutorial() {}

	void initialize();
	void update(float elapsed_time);
	void render(ID3D11DeviceContext* dc);
	void debug_gui();

	bool get_is_tutorial() { return is_tutorial; }
	void set_is_tutorial(bool t) { is_tutorial = t; }

	void next_bord() { select_tutorial_board_num++; }
private:
	std::unique_ptr<SpriteBatch> tex_basic_operation;
	Element basic_operation_param;

	std::unique_ptr<SpriteBatch> tex_skill_select;
	Element skill_select_param;

	std::unique_ptr<SpriteBatch> tex_skill_chant;
	Element skill_chant_param;

	std::unique_ptr<SpriteBatch> tex_press_button;
	Element press_button_param;

	std::unique_ptr<SpriteBatch> tex_open_tutorial;
	Element open_tutorial_param;


	bool is_tutorial;
	int select_tutorial_board_num;

	static constexpr int MAX_TUTORIAL_BOARD_NUM = 2;

	bool display_imgui = false;
};