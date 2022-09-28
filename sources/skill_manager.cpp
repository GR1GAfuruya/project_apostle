#include "skill_manager.h"
#include "user.h"
#include "imgui_include.h"
#include "../external/magic_enum/include/magic_enum.hpp"
//コンストラクタ
SkillManager::SkillManager(Graphics& graphics)
{
	initialize(graphics);
}

//初期化
void SkillManager::initialize(Graphics& graphics)
{
	//身体能力アップスキル
	physical_up_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::PHYCICAL_UP);
	//リジェネスキル
	regeneration_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::REGENERATE);
	//拘束スキル
	restraint_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::RESTRAINNT);
	//魔法弾スキル
	magic_bullet_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::MAGICBULLET);
	//槍スキル
	spears_sea_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::SPEARS_SEA);

	//サポートスキルの枠
	support_skill_slots = { physical_up_skill,regeneration_skill,restraint_skill};
	//攻撃スキルの枠
	attack_skill_slots = { magic_bullet_skill,spears_sea_skill };
	//regeneration_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::REGENERATE);

	//サポートスキル枠の初期化
	for (auto& s : support_skill_slots)
	{
		s->initialize(graphics);
	}

	//攻撃スキル枠の初期化
	for (auto& s : attack_skill_slots)
	{
		s->initialize(graphics);
	}
	support_skill = regeneration_skill.get();
	attack_skill = magic_bullet_skill.get();
}


//更新
void SkillManager::update(Graphics& graphics, float elapsed_time)
{
	//サポートスキル枠の更新
	for (auto& s : support_skill_slots)
	{
		s->update(graphics, elapsed_time);
	}

	//攻撃スキル枠の更新
	for (auto& s : attack_skill_slots)
	{
		s->update(graphics, elapsed_time);
	}
}

//描画
void SkillManager::render(Graphics& graphics)
{
	//サポートスキル枠の描画
	for (auto& s : support_skill_slots)
	{
		s->render(graphics);
	}
	//攻撃スキル枠の描画
	for (auto& s : attack_skill_slots)
	{
		s->render(graphics);
	}
}

//サポートスキル発動
void SkillManager::chant_support_skill(Graphics& graphics)
{
	_ASSERT_EXPR(support_skill != nullptr, L"support_skill is null");

	support_skill->chant(graphics);
}

//攻撃スキル発動
void SkillManager::chant_attack_skill(Graphics& graphics)
{
	_ASSERT_EXPR(attack_skill != nullptr, L"attack_skill is null");

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
	attack_skill = magic_bullet_skill.get();
}

//DebugGui描画
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
