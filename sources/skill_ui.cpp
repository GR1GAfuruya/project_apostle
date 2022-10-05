#include "skill_ui.h"
#include "user.h"
SkillUI::SkillUI(Graphics& graphics, const wchar_t* filename)
{
	skill_slot_icon = make_unique<SpriteBatch>(graphics.get_device().Get(), filename, 8);

}

void SkillUI::initialize(SlotsUi init_param,int slots_num)
{
	slots_ui = init_param;
	this->slots_num = slots_num;
}

void SkillUI::update(Graphics& graphics, float elapsed_time)
{
	
	slots_ui.expansion = skill_select ? expansion_end : expansion_start;
	slots_ui.radius = lerp(slots_ui.radius, slots_ui.expansion, slots_ui.expansion_speed * elapsed_time);

	float alpha = skill_select ? 1.0f : 0.0f;
	slots_ui.color.w = lerp(slots_ui.color.w, alpha, add_alpha_speed * elapsed_time);
	
	
	
	float tar_add_ang = skill_select ? add_ang_end : add_ang_start;
	slots_ui.add_ang = lerp(slots_ui.add_ang, tar_add_ang, add_ang_lerp_speed * elapsed_time);
}

void SkillUI::icon_render(Graphics& graphics)
{
	const int TILE_NUM_X = 4;
	const int TILE_NUM_Y = 4;
	DirectX::XMFLOAT2 tex_size = { skill_slot_icon->get_tex_width() / TILE_NUM_X,
		skill_slot_icon->get_tex_height() / TILE_NUM_Y };

	skill_slot_icon->begin(graphics.get_dc().Get());

	for (int i = 0; i < slots_num; i++)
	{
		//仮で選ばれているスキルを赤っぽく
		slots_ui.color.x = (i == selected_skill_index) ? 5.0f : 1.0f;
		//円周上にアイコンを並べる
		slots_ui.icon_pos =
			Math::circumferential_placement(slots_ui.center_pos, slots_ui.radius, i, slots_num, true, slots_ui.add_ang);
		skill_slot_icon->render(graphics.get_dc().Get(),
			{ slots_ui.icon_pos },//各アイコンの位置
			{ slots_ui.size,slots_ui.size },//アイコンの大きさ
			slots_ui.color,//アイコンの色
			0,//アイコンの角度
			{ (i % TILE_NUM_X) * tex_size.x,(i / TILE_NUM_X) * tex_size.y },//画像の中のアイコンの位置
			tex_size);//画像の切り取りサイズ
	}

	skill_slot_icon->end(graphics.get_dc().Get());
}
void SkillUI::selected_skill_icon_render(Graphics& graphics, DirectX::XMFLOAT2 pos)
{
	const int TILE_NUM_X = 4;
	const int TILE_NUM_Y = 4;
	DirectX::XMFLOAT2 tex_size = { skill_slot_icon->get_tex_width() / TILE_NUM_X,
		skill_slot_icon->get_tex_height() / TILE_NUM_Y };

	skill_slot_icon->begin(graphics.get_dc().Get());
	//円周上にアイコンを並べる
	skill_slot_icon->render(graphics.get_dc().Get(),
		{ pos },//各アイコンの位置
		{ slots_ui.size,slots_ui.size  },//アイコンの大きさ
		{1,1,1,1},//アイコンの色
		0,//アイコンの角度
		{ (selected_skill_index % TILE_NUM_X) * tex_size.x,(selected_skill_index / TILE_NUM_X) * tex_size.y },//画像の中のアイコンの位置
		tex_size);//画像の切り取りサイズ

	skill_slot_icon->end(graphics.get_dc().Get());
}
void SkillUI::debug_gui(string str_id)
{
#if USE_IMGUI

	{
		string name = str_id;
		ImGui::Begin("Skill");
		ImGui::PushID(str_id.c_str());
		ImGui::Text(str_id.c_str());
		ImGui::DragFloat2("center_pos", &slots_ui.center_pos.x);
		ImGui::DragFloat2("icon_pos", &slots_ui.icon_pos.x);
		ImGui::DragFloat("add_ang", &slots_ui.add_ang);
		ImGui::DragFloat("size", &slots_ui.size,0.1f);
		ImGui::DragFloat4("color", &slots_ui.color.x);
		ImGui::Separator();
		ImGui::DragFloat("expansion_start", &expansion_start);
		ImGui::DragFloat("expansion_end", &expansion_end);
		ImGui::DragFloat("add_alpha_speed", &add_alpha_speed);
		ImGui::DragFloat("add_ang_start", &add_ang_start);
		ImGui::DragFloat("add_ang_end", &add_ang_end);
		ImGui::DragFloat("add_ang_lerp_speed", &add_ang_lerp_speed,0.1f);
		ImGui::Separator();
		ImGui::PopID();
		ImGui::End();
	}

#endif

}