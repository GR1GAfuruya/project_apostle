#include "skill_manager.h"
#include "user.h"
#include "imgui_include.h"
#include "../external/magic_enum/include/magic_enum.hpp"
//�R���X�g���N�^
SkillManager::SkillManager(Graphics& graphics)
{
	initialize(graphics);
}

//������
void SkillManager::initialize(Graphics& graphics)
{
	//�g�̔\�̓A�b�v�X�L��
	physical_up_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::PHYCICAL_UP);
	//���W�F�l�X�L��
	regeneration_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::REGENERATE);
	//�S���X�L��
	restraint_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::RESTRAINNT);
	//���@�e�X�L��
	magic_bullet_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::MAGICBULLET);
	//���X�L��
	spears_sea_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::SPEARS_SEA);

	//�T�|�[�g�X�L���̘g
	support_skill_slots = { physical_up_skill,regeneration_skill,restraint_skill};
	//�U���X�L���̘g
	attack_skill_slots = { magic_bullet_skill,spears_sea_skill };
	//regeneration_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::REGENERATE);

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
void SkillManager::set_support_skill()
{
	support_skill = regeneration_skill.get();
}

//�U���X�L�����g�p�g�ɃZ�b�g
void SkillManager::set_attack_skill()
{
	attack_skill = magic_bullet_skill.get();
}

//DebugGui�`��
void SkillManager::debug_gui()
{
#if USE_IMGUI
	
	{
		ImGui::Begin("Skill");
		if (ImGui::Button("set_support_skill"))
		{
			set_support_skill();
		}
		if (ImGui::Button("set_attack_skill"))
		{
			set_attack_skill();
		}

		ImGui::End();
	}

	for (auto& s : support_skill_slots)
	{
		std::string type_name;
		type_name = magic_enum::enum_name<SkillSlot::SkillType>(s->skill_type);
		s->debug_gui(type_name);
	}

	for (auto& s : attack_skill_slots)
	{
		std::string type_name;
		type_name = magic_enum::enum_name<SkillSlot::SkillType>(s->skill_type);
		s->debug_gui(type_name);
	}
#endif
	
}
