#include "support_skill_slot.h"
#include "skill_physical_up.h"
#include "skill_regenerate.h"
#include "skill_restraint.h"

SupportSkillSlot::SupportSkillSlot(Graphics& graphics, SupportSkillType type)
{
	initialize(graphics);
	skill_type = type;
}


void SupportSkillSlot::chant(Graphics& graphics)
{
	//詠唱可能な状態なら
	if (chantable)
	{
		std::unique_ptr<Skill>skill;
		//実態生成
		entity_generation_by_type(graphics, skill, skill_type);
		//クールタイム設定
		skill->initialize(graphics);
		cool_time = skill->get_cool_time();
		//リストに追加
		skills.push_back(std::move(skill));
		chantable = false;
	}
}

void SupportSkillSlot::entity_generation_by_type(Graphics& graphics, std::unique_ptr<Skill>& coffin, SupportSkillType type)
{
	switch (type)
	{
	case SupportSkillType::REGENERATE:
		coffin = std::make_unique<Regenerate>();
		break;
	case SupportSkillType::PHYCICAL_UP:
		coffin = std::make_unique<PhysucalUp>();
		break;
	case SupportSkillType::RESTRAINNT:
		coffin = std::make_unique<Restrain>();
		break;
	
		break;
	default:
		break;
	}
}
