#include "skill_manager.h"
#include "user.h"
#include "imgui_include.h"
//コンストラクタ
SkillManager::SkillManager(Graphics& graphics)
{
	initialize(graphics);
}

//初期化
void SkillManager::initialize(Graphics& graphics)
{
	regeneration_skill = make_shared<SkillSlot>(graphics);
}


//更新
void SkillManager::update(Graphics& graphics, float elapsed_time)
{
	regeneration_skill->update(graphics, elapsed_time);
}

//描画
void SkillManager::render(Graphics& graphics)
{
	regeneration_skill->render(graphics);
}

//サポートスキル発動
void SkillManager::chant_support_skill(Graphics& graphics)
{
	support_skill->chant(graphics);
}

//攻撃スキル発動
void SkillManager::chant_attack_skill(Graphics& graphics)
{
	attack_skill->chant(graphics);
}

//サポートスキルを使用枠にセット
void SkillManager::set_support_skill()
{
	support_skill = regeneration_skill.get();
}

//攻撃スキルを使用枠にセット
void SkillManager::set_attack_skill()
{
	attack_skill = regeneration_skill.get();
}

//DebugGui描画
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
