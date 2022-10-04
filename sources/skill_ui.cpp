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
	const float expansion_start = 30;
	const float expansion_end = 150;
	slots_ui.expansion = skill_select ? expansion_end : expansion_start;
	slots_ui.radius = lerp(slots_ui.radius, slots_ui.expansion, slots_ui.expansion_speed * elapsed_time);

	float alpha = skill_select ? 1.0f : 0.0f;
	slots_ui.color.w = lerp(slots_ui.color.w, alpha, 6.0f * elapsed_time);
	
	
	const float add_ang_start = -120;
	const float add_ang_end = -90;
	float tar_add_ang = skill_select ? add_ang_end : add_ang_start;
	slots_ui.add_ang = lerp(slots_ui.add_ang, tar_add_ang, 6.0f * elapsed_time);
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
		//���őI�΂�Ă���X�L����Ԃ��ۂ�
		slots_ui.color.x = (i == selected_skill_index) ? 5.0f : 1.0f;
		//�~����ɃA�C�R������ׂ�
		slots_ui.icon_pos =
			Math::circumferential_placement(slots_ui.center_pos, slots_ui.radius, i, slots_num, true, slots_ui.add_ang);
		skill_slot_icon->render(graphics.get_dc().Get(),
			{ slots_ui.icon_pos },//�e�A�C�R���̈ʒu
			{ slots_ui.size,slots_ui.size },//�A�C�R���̑傫��
			slots_ui.color,//�A�C�R���̐F
			0,//�A�C�R���̊p�x
			{ (i % TILE_NUM_X) * tex_size.x,(i / TILE_NUM_X) * tex_size.y },//�摜�̒��̃A�C�R���̈ʒu
			tex_size);//�摜�̐؂���T�C�Y
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
	//�~����ɃA�C�R������ׂ�
	skill_slot_icon->render(graphics.get_dc().Get(),
		{ pos },//�e�A�C�R���̈ʒu
		{ slots_ui.size,slots_ui.size  },//�A�C�R���̑傫��
		{1,1,1,1},//�A�C�R���̐F
		0,//�A�C�R���̊p�x
		{ (selected_skill_index % TILE_NUM_X) * tex_size.x,(selected_skill_index / TILE_NUM_X) * tex_size.y },//�摜�̒��̃A�C�R���̈ʒu
		tex_size);//�摜�̐؂���T�C�Y

	skill_slot_icon->end(graphics.get_dc().Get());
}
void SkillUI::debug_gui()
{
#if USE_IMGUI

	{
		ImGui::Begin("Skill");
		ImGui::DragFloat2("center_pos", &slots_ui.center_pos.x);
		ImGui::DragFloat2("icon_pos", &slots_ui.icon_pos.x);
		ImGui::DragFloat("add_ang", &slots_ui.add_ang);
		ImGui::DragFloat("size", &slots_ui.size,0.1f);
		ImGui::DragFloat4("color", &slots_ui.color.x);
		
		ImGui::End();
	}

#endif

}