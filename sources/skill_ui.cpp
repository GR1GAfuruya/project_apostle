#include "skill_ui.h"
#include "user.h"
SkillUI::SkillUI(Graphics& graphics)
{
	skill_slot_icon = make_unique<SpriteBatch>(graphics.get_device().Get(), L"./resources/Sprite/skill_icon.png", 10);

}

void SkillUI::initialize(SlotsUi init_param)
{
	slots_ui = init_param;
	slots_num = 4;
}

void SkillUI::update(Graphics& graphics, float elapsed_time)
{
	slots_ui.expansion = skill_select ? 150 : 0;
	float alpha = skill_select ? 1.0f : 0.0f;
	slots_ui.radius = lerp(slots_ui.radius, slots_ui.expansion, slots_ui.expansion_speed * elapsed_time);
	slots_ui.color.w = lerp(slots_ui.color.w, alpha, 6.0f * elapsed_time);
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
		//�~����ɃA�C�R������ׂ�
		slots_ui.icon_pos =
			Math::circumferential_placement(slots_ui.center_pos, slots_ui.radius, i, TILE_NUM_X * 2, true, slots_ui.add_ang);
		skill_slot_icon->render(graphics.get_dc().Get(),
			{ slots_ui.icon_pos },//�e�A�C�R���̈ʒu
			{ slots_ui.size,slots_ui.size },//�A�C�R���̑傫��
			slots_ui.color,//�A�C�R���̐F
			0,//�A�C�R���̊p�x
			{ i * tex_size.x,0 },//�摜�̒��̃A�C�R���̈ʒu
			tex_size);//�摜�̐؂���T�C�Y
	}
	skill_slot_icon->end(graphics.get_dc().Get());
}
