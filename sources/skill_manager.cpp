#include "skill_manager.h"
#include "device.h"
#include "user.h"
#include "imgui_include.h"
#include "../external/magic_enum/include/magic_enum.hpp"
//�R���X�g���N�^
SkillManager::SkillManager(Graphics& graphics)
{
	sup_slots_ui = make_unique<SkillUI>(graphics);
	atk_slots_ui = make_unique<SkillUI>(graphics);

	//�g�̔\�̓A�b�v�X�L��
	physical_up_skill = make_shared<SupportSkillSlot>(graphics, SP_SKILLTYPE::PHYCICAL_UP);
	//���W�F�l�X�L��
	regeneration_skill = make_shared<SupportSkillSlot>(graphics, SP_SKILLTYPE::REGENERATE);
	//�S���X�L��
	restraint_skill = make_shared<SupportSkillSlot>(graphics, SP_SKILLTYPE::RESTRAINNT);
	//���@�e�X�L��
	magic_bullet_skill = make_shared<AttackSkillSlot>(graphics, AT_SKILLTYPE::MAGICBULLET);
	//���X�L��
	spears_sea_skill = make_shared<AttackSkillSlot>(graphics, AT_SKILLTYPE::SPEARS_SEA);
	initialize(graphics);
}

//������
void SkillManager::initialize(Graphics& graphics)
{
	//�T�|�[�g�X�L���̘g
	support_skill_slots = { physical_up_skill,regeneration_skill,restraint_skill};
	//�U���X�L���̘g
	attack_skill_slots = { magic_bullet_skill,spears_sea_skill };

	//�T�|�[�g�X�L���g�̏�����
	for (auto& s : support_skill_slots)
	{
		s->initialize(graphics);
	}

	//�U���X�L���g�̏�����
	for (auto& s : attack_skill_slots)
	{
		s->initialize(graphics);
	}
	support_skill = regeneration_skill.get();
	attack_skill = magic_bullet_skill.get();

	SkillUI::SlotsUi support_ui_init;
	support_ui_init.center_pos = { 800 ,450 };
	support_ui_init.radius = 0;
	support_ui_init.icon_pos = { 0 ,0 };
	support_ui_init.size = 0.1f;
	support_ui_init.add_ang = -90.0f;
	support_ui_init.expansion_speed = 10.0f;
	support_ui_init.color = { 1,1,1,0 };
	sup_slots_ui->initialize(support_ui_init);

	SkillUI::SlotsUi attack_ui_init;
	attack_ui_init.center_pos = { 800 ,450 };
	attack_ui_init.radius = 0;
	attack_ui_init.icon_pos = { 0 ,0 };
	attack_ui_init.size = 0.1f;
	attack_ui_init.add_ang = -90.0f;
	attack_ui_init.expansion_speed = 10.0f;
	attack_ui_init.color = { 1,0,0,0 };
	atk_slots_ui->initialize(attack_ui_init);
}


//�X�V
void SkillManager::update(Graphics& graphics, float elapsed_time)
{
	//�T�|�[�g�X�L���g�̍X�V
	for (auto& s : support_skill_slots)
	{
		s->update(graphics, elapsed_time);
	}

	//�U���X�L���g�̍X�V
	for (auto& s : attack_skill_slots)
	{
		s->update(graphics, elapsed_time);
	}

	//UI�A�b�v�f�[�g
	sup_slots_ui->update(graphics, elapsed_time);
	atk_slots_ui->update(graphics, elapsed_time);

	//�X�L���I���X�V
	GamePad* game_pad = &Device::instance().get_game_pad();
	//�T�|�[�g�X�L���I�����
	if (game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
	{
		sup_slots_ui->set_skill_select(true);
		atk_slots_ui->set_skill_select(false);
	}
	//�{�^��������������
	else if(game_pad->get_button_up() & GamePad::BTN_RIGHT_SHOULDER)
	{
		sup_slots_ui->set_skill_select(false);
	}
	//�U���X�L���I�����
	if (game_pad->get_button_down() & GamePad::BTN_LEFT_SHOULDER)
	{
		atk_slots_ui->set_skill_select(true);
		sup_slots_ui->set_skill_select(false);

	}
	//�{�^��������������
	else if(game_pad->get_button_up() & GamePad::BTN_LEFT_SHOULDER)
	{
		atk_slots_ui->set_skill_select(false);
	}
	if (game_pad->get_button() & GamePad::BTN_RIGHT_SHOULDER)
	{
		//���͏����擾
		float ax = game_pad->get_axis_RX();
		float ay = game_pad->get_axis_RY();
		DirectX::XMFLOAT2 vec = { ax,ay };
		//���͂���Ă��Ȃ��ꍇ�͔�����
		if (Math::Length(vec) == 0) return;

		set_support_skill(select_skill_slot(vec));
	}

	if (game_pad->get_button() & GamePad::BTN_LEFT_SHOULDER)
	{
		//���͏����擾
		float ax = game_pad->get_axis_RX();
		float ay = game_pad->get_axis_RY();
		DirectX::XMFLOAT2 vec = { ax,ay };
		//���͂���Ă��Ȃ��ꍇ�͔�����
		if (Math::Length(vec) == 0) return;

		set_attack_skill(select_skill_slot(vec));
	}
	

	
}

//�`��
void SkillManager::render(Graphics& graphics)
{
	//�T�|�[�g�X�L���g�̕`��
	for (auto& s : support_skill_slots)
	{
		s->render(graphics);
	}
	//�U���X�L���g�̕`��
	for (auto& s : attack_skill_slots)
	{
		s->render(graphics);
	}
}

void SkillManager::ui_render(Graphics& graphics, float elapsed_time)
{

	sup_slots_ui->icon_render(graphics);
	atk_slots_ui->icon_render(graphics);
}

//�T�|�[�g�X�L������
void SkillManager::chant_support_skill(Graphics& graphics)
{
	_ASSERT_EXPR(support_skill != nullptr, L"support_skill is null");

	support_skill->chant(graphics);
}

//�U���X�L������
void SkillManager::chant_attack_skill(Graphics& graphics)
{
	_ASSERT_EXPR(attack_skill != nullptr, L"attack_skill is null");

	attack_skill->chant(graphics);
}

//�T�|�[�g�X�L�����g�p�g�ɃZ�b�g
void SkillManager::set_support_skill(int skill_index)
{
	skill_index = Math::clamp(skill_index, 0, 2);
	support_skill = support_skill_slots.at(skill_index).get();
}

//�U���X�L�����g�p�g�ɃZ�b�g
void SkillManager::set_attack_skill(int skill_index)
{
	skill_index = Math::clamp(skill_index, 0, 1);
	attack_skill = attack_skill_slots.at(skill_index).get();
}

int SkillManager::select_skill_slot(DirectX::XMFLOAT2 stick_vec)
{
	//���
	DirectX::XMFLOAT2 up = { 0.00001f,1.0f };
	//���K��
	stick_vec = Math::Normalize(stick_vec);
	//�X�e�B�b�N�̂��������Ɗ���Ԃ̊p�x�����߂�
	float dot = Math::Dot(up, stick_vec);
	dot = acosf(dot);
	float deglee_dot = DirectX::XMConvertToDegrees(dot);
	//x�l���}�C�i�X�̏ꍇ�␳
	float add_angle = 180 - deglee_dot;
	float ang = deglee_dot;
	if (stick_vec.x < 0) ang = 180 + add_angle;
	//�X���b�g����̘g�̑傫��������
	float slots_ang_size = 360.0f / ((support_skill_slots.size() + 1) * 2);
	int selected_index;
	//�X���b�g�ԍ�������
	selected_index = ang / slots_ang_size;
	//�s���Ȓl������Ȃ��悤�N�����v
	
	return selected_index;
}

//DebugGui�`��
void SkillManager::debug_gui(Graphics& graphics)
{
#if USE_IMGUI

	{
		ImGui::Begin("Skill");

		{
			//�X�L���I��
			const char* support_slot_item[] = { "physical_up_skill","regeneration_skill","restraint_skill" };
			static int item_current = 0;
			ImGui::Combo("support_slots", &item_current, support_slot_item, IM_ARRAYSIZE(support_slot_item));
			//�Z�b�g

			set_support_skill(item_current);

			
			//����
			if (ImGui::Button("support_skill_chant"))
			{
				chant_support_skill(graphics);
			}
		}

		{
			//�X�L���I��
			const char* attack_slot_item[] = { "magic_bullet_skill","spears_sea_skill" };
			static int item_current2 = 0;
			ImGui::Combo("attack_slots", &item_current2, attack_slot_item, IM_ARRAYSIZE(attack_slot_item));

			set_attack_skill(item_current2);
			
			//����
			if (ImGui::Button("attack_skill_chant"))
			{
				chant_attack_skill(graphics);
			}
		}
		ImGui::End();
	}

	for (auto& s : support_skill_slots)
	{
		std::string type_name;
		type_name = magic_enum::enum_name<SupportSkillSlot::SupportSkillType>(s->skill_type);
		s->debug_gui(type_name);
	}

	for (auto& s : attack_skill_slots)
	{
		std::string type_name;
		type_name = magic_enum::enum_name<AttackSkillSlot::AttackSkillType>(s->skill_type);
		s->debug_gui(type_name);
	}
#endif

}
