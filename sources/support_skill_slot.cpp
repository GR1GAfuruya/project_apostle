#include "support_skill_slot.h"
#include "skill_physical_up.h"
#include "skill_regenerate.h"
#include "skill_restraint.h"

SupportSkillSlot::SupportSkillSlot(Graphics& graphics, SupportSkillType type)
{
	initialize(graphics);
	skill_type = type;
}


void SupportSkillSlot::chant(Graphics& graphics/*,DirectX::XMFLOAT3* init_pos, DirectX::XMFLOAT3* target_pos, int change_state*/)
{
	//�r���\�ȏ�ԂȂ�
	if (chantable)
	{
		std::unique_ptr<Skill>skill;
		//���Ԑ���
		entity_generation_by_type(graphics, skill);
		//�N�[���^�C���ݒ�
		cool_time = skill->get_cool_time();
		//���X�g�ɒǉ�
		skills.push_back(std::move(skill));
		chantable = false;
	}
}

void SupportSkillSlot::entity_generation_by_type(Graphics& graphics, std::unique_ptr<Skill>& coffin)
{
	switch (skill_type)
	{
	case SupportSkillType::REGENERATE:
		coffin = std::make_unique<Regenerate>();
		coffin->initialize(graphics);

		break;
	case SupportSkillType::PHYCICAL_UP:
		coffin = std::make_unique<PhysucalUp>();
		coffin->initialize(graphics);

		break;
	case SupportSkillType::RESTRAINNT:
		coffin = std::make_unique<Restrain>();
		break;
	default:
		break;
	}
}
