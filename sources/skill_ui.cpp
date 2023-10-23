#include "skill_ui.h"
#include "user.h"
#include "device.h"
//==============================================================
// 
// �R���X�g���N�^
// 
//==============================================================
SkillUI::SkillUI(const wchar_t* filename)
{
	Graphics& graphics = Graphics::instance();
	//�X�L���̃A�C�R��
	skill_slot_icon = make_unique<SpriteBatch>(graphics.get_device().Get(), filename, 8);
	//�N�[���^�C���̃Q�[�W�i���͉��Ńv���C���[��HP�摜���g�p�j
	cool_time_gauge = make_unique<GaugeUi>(L"./resources/Sprite/UI/Player/player_hp_bar_back.png",
		L"./resources/Sprite/UI/Player/bar.png",
		nullptr);
	cool_time_gauge->set_angle(-90);

}
//==============================================================
// 
// ������
// 
//==============================================================
void SkillUI::initialize(SlotsUi init_param, int slots_num)
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
//�X�V����
// 
//==============================================================
void SkillUI::update(float elapsed_time)
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
//�`�揈���i�X�L�����X�g�̃A�C�R���j
// 
//==============================================================
void SkillUI::icon_render()
{
	Graphics& graphics = Graphics::instance();
	const int TILE_NUM_X = 4;
	const int TILE_NUM_Y = 4;
	DirectX::XMFLOAT2 tex_size = { (skill_slot_icon->get_tex_width() / TILE_NUM_X) ,
		(skill_slot_icon->get_tex_height() / TILE_NUM_Y) };

	skill_slot_icon->begin(graphics.get_dc().Get());

	for (int i = 0; i < slots_num; i++)
	{
		//���őI�΂�Ă���X�L����Ԃ��ۂ�
		slots_ui.color.x = (i == selected_skill_index) ? 5.0f : 1.0f;
		//�~����ɃA�C�R������ׂ�
		slots_ui.icon_pos =
			Math::circumferential_placement(slots_ui.center_pos, slots_ui.radius, i, slots_num, true, slots_ui.add_ang);
		skill_slot_icon->render(graphics.get_dc().Get(),
			slots_ui.icon_pos,//�e�A�C�R���̈ʒu
			{ slots_ui.size , slots_ui.size },//�A�C�R���̑傫��
			slots_ui.color,//�A�C�R���̐F
			0,//�A�C�R���̊p�x
			{ (i % TILE_NUM_X) * tex_size.x,(i / TILE_NUM_Y) * tex_size.y },//�摜�̒��̃A�C�R���̈ʒu
			tex_size);//�摜�̐؂���T�C�Y
	}

	skill_slot_icon->end(graphics.get_dc().Get());
	//���
	{
		//�X�L���I���X�V
		GamePad* game_pad = &Device::instance().get_game_pad();
		//���͏����擾
		float ax = game_pad->get_axis_RX();
		float ay = game_pad->get_axis_RY();
		DirectX::XMFLOAT2 stick_vec = { ax,ay };

		float arrow_angle;
		DirectX::XMFLOAT2 standard_axis = { 0, 1 };
		arrow_angle = Math::Dot(stick_vec, standard_axis);
		arrow_angle = acosf(arrow_angle);
		float cross{ stick_vec.x * standard_axis.y - stick_vec.y * standard_axis.x };
		if (cross < 0) arrow_angle = 360.0f - arrow_angle;

	}
}
//==============================================================
// 
//�`�揈���@(���ݑI������Ă���X�L���̃A�C�R���\��)
// 
//==============================================================
void SkillUI::selected_skill_icon_render(DirectX::XMFLOAT2 pos)
{
	Graphics& graphics = Graphics::instance();
	const int TILE_NUM_X = 4;
	const int TILE_NUM_Y = 4;
	DirectX::XMFLOAT2 tex_size = { skill_slot_icon->get_tex_width() / TILE_NUM_X,
		skill_slot_icon->get_tex_height() / TILE_NUM_Y };

	skill_slot_icon->begin(graphics.get_dc().Get());
	//�~����ɃA�C�R������ׂ�
	skill_slot_icon->render(graphics.get_dc().Get(),
		pos,//�e�A�C�R���̈ʒu
		{ slots_ui.size , slots_ui.size },//�A�C�R���̑傫��
		{ 1,1,1,1 },//�A�C�R���̐F
		0,//�A�C�R���̊p�x
		{ (selected_skill_index % TILE_NUM_X) * tex_size.x ,(selected_skill_index / TILE_NUM_Y) * tex_size.y },//�摜�̒��̃A�C�R���̈ʒu
		tex_size);//�摜�̐؂���T�C�Y

	skill_slot_icon->end(graphics.get_dc().Get());
}

//==============================================================
// 
//�f�o�b�OGUI�\��
// 
//==============================================================
void SkillUI::debug_gui(string str_id)
{
#if USE_IMGUI
	imgui_menu_bar("UI", "skill", display_imgui);
	if (display_imgui)
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
		ImGui::DragFloat("size", &slots_ui.size, 0.1f);
		ImGui::DragFloat4("color", &slots_ui.color.x);
		ImGui::DragFloat("expansion_start", &expansion_start);
		ImGui::DragFloat("expansion_end", &expansion_end);
		ImGui::DragFloat("add_alpha_speed", &add_alpha_speed);
		ImGui::DragFloat("add_ang_start", &add_ang_start);
		ImGui::DragFloat("add_ang_end", &add_ang_end);
		ImGui::DragFloat("add_ang_lerp_speed", &add_ang_lerp_speed, 0.1f);
		ImGui::DragFloat("add_ang_lerp_speed", &add_ang_lerp_speed, 0.1f);
		//ImGui::DragFloat2("arrow_pos", &arrow_pos.x,0.1f);
		//ImGui::DragFloat2("arrow_scale", &arrow_scale.x,0.1f);
		ImGui::DragFloat2("cool_time_gauge_pos", &slots_ui.cool_time_gauge_pos.x, 0.1f);
		ImGui::Separator();
		ImGui::PopID();
		ImGui::End();
	}

#endif

}


//==============================================================
// 
//�N�[���^�C���\��
// 
//==============================================================
void SkillUI::cool_time_render(float elapsed_time, float skill_cool_per)
{
	Graphics& graphics = Graphics::instance();
	//�ʒu�ݒ�
	cool_time_gauge->set_position(slots_ui.cool_time_gauge_pos);
	//�c������
	const float ang = -90;
	cool_time_gauge->set_angle(ang);
	//�T�C�Y�ݒ�
	const DirectX::XMFLOAT2 t_size = { 60.0f,10.0f };
	const DirectX::XMFLOAT4 cool_time_color = { 10.0f,10.0f,10.0f,1.0f };

	cool_time_gauge->set_tex_size(t_size);
	cool_time_gauge->set_color(cool_time_color);
	//���ݒ�
	cool_time_gauge->set_percent(skill_cool_per);
	cool_time_gauge->render(graphics.get_dc().Get());
}
