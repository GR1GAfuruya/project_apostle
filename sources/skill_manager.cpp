#include "skill_manager.h"
#include "device.h"
#include "user.h"
#include "imgui_include.h"
#include "../external/magic_enum/include/magic_enum.hpp"
//==============================================================
// 
// �R���X�g���N�^
// 
//==============================================================
SkillManager::SkillManager(Graphics& graphics)
{
	sup_slots_ui = make_unique<SkillUI>(graphics, L"./resources/Sprite/UI/Skill/support_skill_icon.png");
	atk_slots_ui = make_unique<SkillUI>(graphics, L"./resources/Sprite/UI/Skill/attack_skill_icon.png");

	//�g�̔\�̓A�b�v�X�L��
	physical_up = make_unique<PhysicalUpLauncher>(graphics);
	
	//���W�F�l�X�L��
	regenerate = make_unique<RegenerateLauncher>(graphics);
	 
	//�S���X�L��
	restraint = make_unique<RestraintLauncher>(graphics);
	
	//���@�e�X�L��
	magick_bullet = make_unique<MagicBulletLauncher>(graphics);
	
	//���X�L��
	spear_sea = make_unique<SpearSeaLauncher>(graphics);

	initialize(graphics);
}

//==============================================================
// 
// ������
// 
//==============================================================
void SkillManager::initialize(Graphics& graphics)
{
	//�X�L���̏�����
	{
		//�g�̔\�̓A�b�v�X�L��
		physical_up->initialize(graphics);
		//���W�F�l�X�L��
		regenerate->initialize(graphics);
		//�S���X�L��
		restraint->initialize(graphics);
		//���@�e�X�L��
		magick_bullet->initialize(graphics);
		//���X�L��
		spear_sea->initialize(graphics);
	}
	//UI�����ݒ�
	{
		SkillUI::SlotsUi support_ui_init{};
		support_ui_init.center_pos = { 800 ,450 };
		support_ui_init.radius = 0;
		support_ui_init.icon_pos = { 0 ,0 };
		support_ui_init.size = 0.1f;
		support_ui_init.add_ang = -90.0f;
		support_ui_init.expansion_speed = 10.0f;
		support_ui_init.color = { 1,1,1,0 };
		sup_slots_ui->initialize(support_ui_init, static_cast<int>(SupportSkillType::SUP_SKILL_MAX));

		SkillUI::SlotsUi attack_ui_init{};
		attack_ui_init.center_pos = { 800 ,450 };
		attack_ui_init.radius = 0;
		attack_ui_init.icon_pos = { 0 ,0 };
		attack_ui_init.size = 0.1f;
		attack_ui_init.add_ang = -90.0f;
		attack_ui_init.expansion_speed = 10.0f;
		attack_ui_init.color = { 1,1,1,0 };
		atk_slots_ui->initialize(attack_ui_init, static_cast<int>(AttackSkillType::ATK_SKILL_MAX));
	}
}


//==============================================================
// 
//�X�V����
// 
//==============================================================
void SkillManager::update(Graphics& graphics, float elapsed_time)
{
	//�X�L���̏�����
	{
		//�g�̔\�̓A�b�v�X�L��
		physical_up->update(graphics, elapsed_time);
		//���W�F�l�X�L��
		regenerate->update(graphics, elapsed_time);
		//�S���X�L��
		restraint->update(graphics, elapsed_time);
		//���@�e�X�L��
		magick_bullet->update(graphics, elapsed_time);
		//���X�L��
		spear_sea->update(graphics, elapsed_time);
	}

	//UI�A�b�v�f�[�g
	sup_slots_ui->update(graphics, elapsed_time);
	sup_slots_ui->set_selected_skill_index(static_cast<int>(selected_sup_skill_type));
	atk_slots_ui->update(graphics, elapsed_time);
	atk_slots_ui->set_selected_skill_index(static_cast<int>(selected_atk_skill_type));

	//�X�L���I���X�V
	GamePad* game_pad = &Device::instance().get_game_pad();
	//�T�|�[�g�X�L���I�����
	if (game_pad->get_button_down() & GamePad::BTN_LEFT_SHOULDER)
	{
		//�T�|�[�g�X�L���X���b�g��UI��W�J
		sup_slots_ui->set_skill_select(true);
		//�U���X�L���X���b�g��UI�����
		atk_slots_ui->set_skill_select(false);
		is_selecting_support_skill = true;
	}
	//�{�^��������������
	else if(game_pad->get_button_up() & GamePad::BTN_LEFT_SHOULDER)
	{
		//�T�|�[�g�X�L���X���b�g��UI�����
		sup_slots_ui->set_skill_select(false);
		is_selecting_support_skill = false;
	}

	//�U���X�L���I�����
	if (game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
	{
		//�U���X�L���X���b�g��UI��W�J
		atk_slots_ui->set_skill_select(true);
		//�T�|�[�g�X�L���X���b�g��UI�����
		sup_slots_ui->set_skill_select(false);
		is_selecting_attack_skill = true;

	}
	//�{�^��������������
	else if(game_pad->get_button_up() & GamePad::BTN_RIGHT_SHOULDER)
	{
		atk_slots_ui->set_skill_select(false);
		is_selecting_attack_skill = false;
	}

	//�X�L����I��ŃZ�b�g���鏈��
	{
		if (game_pad->get_button() & GamePad::BTN_LEFT_SHOULDER)
		{
			//�T�|�[�g�X�L��
			//���͏����擾
			float ax = game_pad->get_axis_RX();
			float ay = game_pad->get_axis_RY();
			DirectX::XMFLOAT2 vec = { ax,ay };
			//���͂���Ă��Ȃ��ꍇ�͔�����
			if (Math::Length(vec) == 0) return;

			set_support_skill(select_skill_slot(vec, static_cast<int>(SupportSkillType::SUP_SKILL_MAX)));
		}

		//�U���X�L���X�L��
		if (game_pad->get_button() & GamePad::BTN_RIGHT_SHOULDER)
		{
			//���͏����擾
			float ax = game_pad->get_axis_RX();
			float ay = game_pad->get_axis_RY();
			DirectX::XMFLOAT2 vec = { ax,ay };
			//���͂���Ă��Ȃ��ꍇ�͔�����
			if (Math::Length(vec) == 0) return;

			set_attack_skill(select_skill_slot(vec, static_cast<int>(AttackSkillType::ATK_SKILL_MAX)));
		}
	}

	
}

//==============================================================
// 
//�`�揈��
// 
//==============================================================
void SkillManager::render(Graphics& graphics)
{
	//�X�L���̕`��
	{
		//�g�̔\�̓A�b�v�X�L��
		physical_up->render(graphics);
		//���W�F�l�X�L��
		regenerate->render(graphics);
		//�S���X�L��
		restraint->render(graphics);
		//���@�e�X�L��
		magick_bullet->render(graphics);
		//���X�L��
		spear_sea->render(graphics);
	}
}
//==============================================================
// 
//�`�揈���iUI�j
// 
//==============================================================
void SkillManager::ui_render(Graphics& graphics, float elapsed_time)
{
#if USE_IMGUI
	static DirectX::XMFLOAT2 pos1 = { 1000.0f,600.0f };
	static DirectX::XMFLOAT2 pos2 = { 1120.0f,600.0f };
	if (display_imgui)
	{
		ImGui::Begin("Skill");
		ImGui::DragFloat2("pos1", &pos1.x);
		ImGui::DragFloat2("pos2", &pos2.x);
		ImGui::End();
	}
#else
	DirectX::XMFLOAT2 pos1 = { 1000.0f,600.0f };
	DirectX::XMFLOAT2 pos2 = { 1120.0f,600.0f };
#endif
	sup_slots_ui->icon_render(graphics);
	sup_slots_ui->selected_skill_icon_render(graphics, pos1);
	atk_slots_ui->icon_render(graphics);
	atk_slots_ui->selected_skill_icon_render(graphics, pos2);
}

//==============================================================
// 
//PhycicalUp����
// 
//==============================================================
bool SkillManager::chant_physical_up(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, float* add_run_speed, float* add_jump_speed)
{
	return physical_up->chant(graphics, launch_pos, add_run_speed, add_jump_speed);
}
//==============================================================
// 
//Regenerate����
// 
//==============================================================
bool SkillManager::chant_regenerate(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, int* health)
{
	return regenerate->chant(graphics, launch_pos, health);
}
//==============================================================
// 
//Restraint����
// 
//==============================================================
bool SkillManager::chant_restraint(Graphics& graphics, DirectX::XMFLOAT3* target_pos, float* down_speed)
{
	return restraint->chant(graphics, target_pos, target_pos);
}
//==============================================================
// 
//MagickBullet����
// 
//==============================================================
bool SkillManager::chant_magic_bullet(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 dir)
{
	return magick_bullet->chant(graphics, launch_pos, dir);
}
//==============================================================
// 
//SpearSea����
// 
//==============================================================
bool SkillManager::chant_spear_sea(Graphics& graphics, DirectX::XMFLOAT3 launch_pos)
{
	return spear_sea->chant(graphics, launch_pos);
}


//==============================================================
// 
//���@�e�ƓG�̓����蔻��
// 
//==============================================================
void SkillManager::judge_magic_bullet_vs_enemy(Capsule object_colider, AddDamageFunc damaged_func)
{
	magick_bullet->skill_object_hit_judgment(object_colider, damaged_func);
}

//==============================================================
// 
//�T�|�[�g�X�L�����g�p�g�ɃZ�b�g
// 
//==============================================================
void SkillManager::set_support_skill(int skill_index)
{
	selected_sup_skill_type = static_cast<SupportSkillType>(skill_index);
}
//==============================================================
// 
//�U���X�L�����g�p�g�ɃZ�b�g
// 
//==============================================================
void SkillManager::set_attack_skill(int skill_index)
{
	selected_atk_skill_type = static_cast<AttackSkillType>(skill_index);
}

//==============================================================
// 
//�R���g���[���[�̉E�X�e�B�b�N�ŃX�L����I��
// 
//==============================================================
int SkillManager::select_skill_slot(DirectX::XMFLOAT2 stick_vec, int slot_num)
{
	//�X���b�g����̘g�̑傫��������
	float slots_ang_size = 360.0f / slot_num;
	//��� ���삵�₷���悤�ɃX���b�g����̘g�̑傫����2�����ɌX����
	float correction_standard_ang = -DirectX::XMConvertToRadians(slots_ang_size / 2);
	DirectX::XMFLOAT2 standard_axis = { sinf(correction_standard_ang), cosf(correction_standard_ang)};
	//���K��
	stick_vec = Math::Normalize(stick_vec);
	//�X�e�B�b�N�̂��������Ɗ���Ԃ̊p�x�����߂�
	float dot = Math::Dot(standard_axis, stick_vec);
	dot = acosf(dot);
	float deglee_dot = DirectX::XMConvertToDegrees(dot);
	//Dot��180�x���������Ƃ��ɕ␳��������360�x�ɑΉ��ł���悤�ɂ���
	float ang = deglee_dot;
	float cross{ stick_vec.x * standard_axis.y - stick_vec.y * standard_axis.x };
	if (cross < 0) ang = 360.0f - ang;
	//�X���b�g�ԍ�������
	int selected_index;
	selected_index = static_cast<int>(ang / slots_ang_size);
	return selected_index;
}

//==============================================================
// 
//�f�o�b�OGUI�\��
// 
//==============================================================
void SkillManager::debug_gui(Graphics& graphics)
{
#if USE_IMGUI
	imgui_menu_bar("Skill", "SkillManager", display_imgui);
	if (display_imgui)
	{

		ImGui::Begin("Skill");

		{
			//���݂̃X�L���\��
			{
				std::string type_name;
				type_name = magic_enum::enum_name<SupportSkillType>(selected_sup_skill_type);
				ImGui::Text(type_name.c_str());
				ImGui::Separator();

			}
			//�X�L���I��
			const char* support_slot_item[] = { "physical_up_skill","regeneration_skill","restraint_skill" };
			static int item_current = 0;
			ImGui::Separator();
			ImGui::Combo("support_slots", &item_current, support_slot_item, IM_ARRAYSIZE(support_slot_item));
			
			
			//����
			if (ImGui::Button("set_support_skill"))
			{
				//�Z�b�g
				set_support_skill(item_current);
			}
			ImGui::Separator();
		}
		//���݂̃X�L���\��
		{
			std::string type_name;
			type_name = magic_enum::enum_name<AttackSkillType>(selected_atk_skill_type);
			ImGui::Text(type_name.c_str());
			ImGui::Separator();

		}


		{
			//�X�L���I��
			const char* attack_slot_item[] = { "magic_bullet_skill","spears_sea_skill" };
			static int item_current2 = 0;
			ImGui::Separator();
			ImGui::Combo("attack_slots", &item_current2, attack_slot_item, IM_ARRAYSIZE(attack_slot_item));

			
			
			//����
			if (ImGui::Button("set_attack_skill"))
			{
				set_attack_skill(item_current2);
			}
				ImGui::Separator();
		}
		ImGui::End();
	}

	
	//�g�̔\�̓A�b�v�X�L��
	physical_up->debug_gui();
	//���W�F�l�X�L��
	regenerate->debug_gui();
	//�S���X�L��
	restraint->debug_gui();
	//���@�e�X�L��
	magick_bullet->debug_gui();
	//���X�L��
	spear_sea->debug_gui();
	

	//UIdebugGUI
	atk_slots_ui->debug_gui("atk_slots_ui");
	sup_slots_ui->debug_gui("sup_slots_ui");
#endif

}
