#include "skill_slot.h"

void SkillSlot::initialize(Graphics& graphics)
{
	 cool_time = 0;
	//詠唱可能か
	 chantable = true;

	 cool_time_attenuation_speed = 1.0f;
	 icon;
	 skill.clear();
}

void SkillSlot::update(Graphics& graphics, float elapsed_time)
{
	for (auto& s : skill)
	{
		s->update(graphics, elapsed_time);
	}
	//寿命が尽きたスキルを削除
	skill.erase(std::remove_if(std::begin(skill), std::end(skill),
		[](std::unique_ptr<Skill> const& s) { return s->is_skill_end(); }), std::end(skill));

	//クールタイム更新
	if (cool_time < 0)
	{
		cool_time -= cool_time_attenuation_speed * elapsed_time;
	}

	if (cool_time >= 0)
	{
		cool_time = 0;
		chantable = true;
	}
}

void SkillSlot::render(Graphics& graphics)
{
	for (auto& s : skill)
	{
		s->render(graphics);
	}
}

void SkillSlot::debug_gui()
{
	int count = 0;
	for (auto& s : skill)
	{
		s->debug_gui(to_string(count).c_str());
		count++;
	}
}

void SkillSlot::chant(Graphics& graphics)
{
	std::unique_ptr<Skill>skill1 = std::make_unique<Skill>(graphics);
	cool_time = skill1->COOL_TIME;
	skill.push_back(std::move(skill1));
	chantable = false;
}
