#include "skill_manager.h"
#include "user.h"
#include "imgui_include.h"
//�R���X�g���N�^
SkillManager::SkillManager(Graphics& graphics)
{
	initialize(graphics);
}

//������
void SkillManager::initialize(Graphics& graphics)
{
	regeneration_skill = make_shared<SkillSlot>(graphics);
}


//�X�V
void SkillManager::update(Graphics& graphics, float elapsed_time)
{
	regeneration_skill->update(graphics, elapsed_time);
}

//�`��
void SkillManager::render(Graphics& graphics)
{
	regeneration_skill->render(graphics);
}

//�T�|�[�g�X�L������
void SkillManager::chant_support_skill(Graphics& graphics)
{
	support_skill->chant(graphics);
}

//�U���X�L������
void SkillManager::chant_attack_skill(Graphics& graphics)
{
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
	attack_skill = regeneration_skill.get();
}

//DebugGui�`��
void SkillManager::debug_gui()
{
#if USE_IMGUI
	imgui_menu_bar("charactor", "skill", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("skillManager");
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
#endif
	regeneration_skill->debug_gui();
}
