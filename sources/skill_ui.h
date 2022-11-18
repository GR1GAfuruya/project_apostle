#pragma once
#include "graphics.h"
#include "sprite_batch.h"
//==============================================================
// 
// SkillUIクラス :スキルのUIに関する部分をまとめたクラス
// 
//==============================================================
class SkillUI
{
public:
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================
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
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	SkillUI(Graphics& graphics, const wchar_t* filename);
	~SkillUI(){}
	//初期化
	void initialize(SlotsUi init_param, int slots_num);
	//更新
	void update(Graphics& graphics,float elapsed_time);
	//選択候補のスキルを表示
	void icon_render(Graphics& graphics);
	//現在セットされているスキルのアイコンを表示
	void selected_skill_icon_render(Graphics& graphics, DirectX::XMFLOAT2 pos);
	//デバッグGUI
	void debug_gui(string str_id);
	//スキル選択状態かを設定
	void set_skill_select(bool s) { skill_select = s; };
	//選択されているスキルの番号を設定
	void set_selected_skill_index(int i) { selected_skill_index = i; };
private:
	//スキルアイコンの参照画像
	std::unique_ptr<SpriteBatch> skill_slot_icon;
	//描画に関するパラメーター
	SlotsUi slots_ui;
	//スキルが選ばれているか
	bool skill_select;
	//選択されているスキルの番号
	int selected_skill_index;
	//スキルの総数
	int slots_num;
	//スキルアイコンの展開に関するパラメーター
	float expansion_start;
	float expansion_end;
	float add_alpha_speed;

	float add_ang_start;
	float add_ang_end;
	float add_ang_lerp_speed;
	//Imguiを表示するか
	bool display_imgui = false;
};