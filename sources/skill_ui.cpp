#include "skill_ui.h"
#include "user.h"
//==============================================================
// 
// コンストラクタ
// 
//==============================================================
SkillUI::SkillUI(Graphics& graphics, const wchar_t* filename)
{
	//スキルのアイコン
	skill_slot_icon = make_unique<SpriteBatch>(graphics.get_device().Get(), filename, 8);
	//クールタイムのゲージ（今は仮でプレイヤーのHP画像を使用）
	cool_time_gauge = make_unique<GaugeUi>(graphics, L"./resources/Sprite/UI/Player/player_hp_bar_back.png",
		L"./resources/Sprite/UI/Player/bar.png",
		nullptr);
	cool_time_gauge->set_angle(-90);

}
//==============================================================
// 
// 初期化
// 
//==============================================================
void SkillUI::initialize(SlotsUi init_param,int slots_num)
{
	slots_ui = init_param;
	this->slots_num = slots_num;

	 skill_select = false;
	 selected_skill_index = 0;

	expansion_start = 30;
	expansion_end = 150;
	add_alpha_speed = 5;

	add_ang_start = -120;
	add_ang_end = -90;
	add_ang_lerp_speed = 6;

}

//==============================================================
// 
//更新処理
// 
//==============================================================
void SkillUI::update(Graphics& graphics, float elapsed_time)
{
	
	slots_ui.expansion = skill_select ? expansion_end : expansion_start;
	slots_ui.radius = lerp(slots_ui.radius, slots_ui.expansion, slots_ui.expansion_speed * elapsed_time);

	float alpha = skill_select ? 1.0f : 0.0f;
	slots_ui.color.w = lerp(slots_ui.color.w, alpha, add_alpha_speed * elapsed_time);
	
	
	
	float tar_add_ang = skill_select ? add_ang_end : add_ang_start;
	slots_ui.add_ang = lerp(slots_ui.add_ang, tar_add_ang, add_ang_lerp_speed * elapsed_time);
}

//==============================================================
// 
//描画処理（スキルリストのアイコン）
// 
//==============================================================
void SkillUI::icon_render(Graphics& graphics)
{
	const int TILE_NUM_X = 4;
	const int TILE_NUM_Y = 4;
	DirectX::XMFLOAT2 tex_size = { (skill_slot_icon->get_tex_width() / TILE_NUM_X) ,
		(skill_slot_icon->get_tex_height() / TILE_NUM_Y)  };

	skill_slot_icon->begin(graphics.get_dc().Get());

	for (int i = 0; i < slots_num; i++)
	{
		//仮で選ばれているスキルを赤っぽく
		slots_ui.color.x = (i == selected_skill_index) ? 5.0f : 1.0f;
		//円周上にアイコンを並べる
		slots_ui.icon_pos =
			Math::circumferential_placement(slots_ui.center_pos, slots_ui.radius, i, slots_num, true, slots_ui.add_ang);
		skill_slot_icon->render(graphics.get_dc().Get(),
			slots_ui.icon_pos,//各アイコンの位置
			{ slots_ui.size , slots_ui.size  },//アイコンの大きさ
			slots_ui.color,//アイコンの色
			0,//アイコンの角度
			{ (i % TILE_NUM_X) * tex_size.x,(i / TILE_NUM_Y) * tex_size.y},//画像の中のアイコンの位置
			tex_size);//画像の切り取りサイズ
	}

	skill_slot_icon->end(graphics.get_dc().Get());
}
//==============================================================
// 
//描画処理　(現在選択されているスキルのアイコン表示)
// 
//==============================================================
void SkillUI::selected_skill_icon_render(Graphics& graphics, DirectX::XMFLOAT2 pos)
{
	const int TILE_NUM_X = 4;
	const int TILE_NUM_Y = 4;
	DirectX::XMFLOAT2 tex_size = { skill_slot_icon->get_tex_width() / TILE_NUM_X,
		skill_slot_icon->get_tex_height() / TILE_NUM_Y };

	skill_slot_icon->begin(graphics.get_dc().Get());
	//円周上にアイコンを並べる
	skill_slot_icon->render(graphics.get_dc().Get(),
		pos,//各アイコンの位置
		{  slots_ui.size , slots_ui.size },//アイコンの大きさ
		{1,1,1,1},//アイコンの色
		0,//アイコンの角度
		{ (selected_skill_index % TILE_NUM_X) * tex_size.x ,(selected_skill_index / TILE_NUM_Y) * tex_size.y  },//画像の中のアイコンの位置
		tex_size);//画像の切り取りサイズ

	skill_slot_icon->end(graphics.get_dc().Get());
}

//==============================================================
// 
//デバッグGUI表示
// 
//==============================================================
void SkillUI::debug_gui(string str_id)
{
#if USE_IMGUI
	imgui_menu_bar("UI", "skill", display_imgui);
	if(display_imgui)
	{
		string name = str_id;
		ImGui::Begin("Skill");
		ImGui::PushID(str_id.c_str());
		ImGui::Separator();
		ImGui::Text(str_id.c_str());
		ImGui::Separator();
		ImGui::DragFloat2("center_pos", &slots_ui.center_pos.x);
		ImGui::DragFloat2("icon_pos", &slots_ui.icon_pos.x);
		ImGui::DragFloat("add_ang", &slots_ui.add_ang);
		ImGui::DragFloat("size", &slots_ui.size,0.1f);
		ImGui::DragFloat4("color", &slots_ui.color.x);
		ImGui::DragFloat("expansion_start", &expansion_start);
		ImGui::DragFloat("expansion_end", &expansion_end);
		ImGui::DragFloat("add_alpha_speed", &add_alpha_speed);
		ImGui::DragFloat("add_ang_start", &add_ang_start);
		ImGui::DragFloat("add_ang_end", &add_ang_end);
		ImGui::DragFloat("add_ang_lerp_speed", &add_ang_lerp_speed,0.1f);
		ImGui::DragFloat2("cool_time_gauge_pos", &slots_ui.cool_time_gauge_pos.x,0.1f);
		ImGui::Separator();
		ImGui::PopID();
		ImGui::End();
	}

#endif

}


//==============================================================
// 
//クールタイム表示
// 
//==============================================================
void SkillUI::cool_time_render(Graphics& graphics, float elapsed_time, float skill_cool_per)
{
	//位置設定
	cool_time_gauge->set_position(slots_ui.cool_time_gauge_pos);
	//縦向きに
	const float ang = -90;
	cool_time_gauge->set_angle(ang);
	//サイズ設定
	const DirectX::XMFLOAT2 t_size = { 60.0f,10.0f };
	const DirectX::XMFLOAT4 cool_time_color = { 10.0f,10.0f,10.0f,1.0f };

	cool_time_gauge->set_tex_size(t_size);
	cool_time_gauge->set_color(cool_time_color);
	//％設定
	cool_time_gauge->set_percent(skill_cool_per);
	cool_time_gauge->render(graphics.get_dc().Get());
}
