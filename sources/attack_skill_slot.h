#pragma once
#include "skill_slot.h"
#define AT_SKILLTYPE AttackSkillSlot::AttackSkillType

class AttackSkillSlot : public SkillSlot
{
public:
	//==============================================================
	// 
	// ç\ë¢ëÃÅAóÒãìå^
	// 
	//==============================================================

	enum class AttackSkillType
	{
		MAGICBULLET,
		SPEARS_SEA
	};
public:
	//==============================================================
	// 
	// publicä÷êî
	// 
	//==============================================================
	AttackSkillSlot(Graphics& graphics, AttackSkillType type);
	~AttackSkillSlot() {}

	void chant(Graphics& graphics);
	void entity_generation_by_type(Graphics& graphics, std::unique_ptr<Skill>& coffin, AttackSkillType type);
	//==============================================================
	// 
	// ïœêî
	// 
	//==============================================================

	AttackSkillType skill_type;
	//==============================================================
	// 
	// íËêî
	// 
	//==============================================================
};
