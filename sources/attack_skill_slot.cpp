#include "attack_skill_slot.h"
#include "skill_magic_bullet.h"
#include "skill_spears_sea.h"

AttackSkillSlot::AttackSkillSlot(Graphics& graphics, AttackSkillType type)
{
	initialize(graphics);
	skill_type = type;
}

void AttackSkillSlot::chant(Graphics& graphics)
{
	//�r���\�ȏ�ԂȂ�
	if (chantable)
	{
		std::unique_ptr<Skill>skill;
		//���Ԑ���
		entity_generation_by_type(graphics, skill, skill_type);
		//�N�[���^�C���ݒ�
		skill->initialize(graphics);
		cool_time = skill->get_cool_time();
		//���X�g�ɒǉ�
		skills.push_back(std::move(skill));
		chantable = false;
	}
}

void AttackSkillSlot::entity_generation_by_type(Graphics& graphics, std::unique_ptr<Skill>& coffin, AttackSkillType type)
{
	switch (type)
	{
	case AttackSkillType::MAGICBULLET:
		coffin = std::make_unique<MagicBullet>();
		break;
	case AttackSkillType::SPEARS_SEA:
		coffin = std::make_unique<SpearsSea>();
		break;
	
	default:
		break;
	}
}
