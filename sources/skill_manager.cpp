#include "skill_manager.h"

void SkillManager::initialize(Graphics& graphics)
{
	regeneration_skill = make_shared<SkillSlot>();
}

void SkillManager::update(Graphics& graphics, float elapsed_time)
{
	regeneration_skill->update(graphics, elapsed_time);
}

void SkillManager::render(Graphics& graphics)
{
	regeneration_skill->render(graphics);
}

void SkillManager::chant_support_skill(Graphics& graphics)
{
	support_skill->chant(graphics);
}

void SkillManager::chant_attack_skill(Graphics& graphics)
{
	attack_skill->chant(graphics);
}

void SkillManager::set_support_skill()
{
	support_skill = regeneration_skill.get();
}

void SkillManager::set_attack_skill()
{
	attack_skill = regeneration_skill.get();
}

void SkillManager::debug_gui()
{
	regeneration_skill->debug_gui();
}
