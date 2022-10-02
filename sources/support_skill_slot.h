#pragma once
#include "skill_slot.h"
#define SP_SKILLTYPE SupportSkillSlot::SupportSkillType

class SupportSkillSlot : public SkillSlot
{
public:
	//==============================================================
	// 
	// ç\ë¢ëÃÅAóÒãìå^
	// 
	//==============================================================
	enum class SupportSkillType
	{
		REGENERATE,
		PHYCICAL_UP,
		RESTRAINNT,
	};


	//==============================================================
	// 
	// publicä÷êî
	// 
	//==============================================================
	SupportSkillSlot(Graphics& graphics, SupportSkillType type);
	~SupportSkillSlot() {}
	void chant(Graphics& graphics);
	//==============================================================
	// 
	// privateä÷êî
	// 
	//==============================================================
	void entity_generation_by_type(Graphics& graphics, std::unique_ptr<Skill>& coffin, SupportSkillType type) ;
	//==============================================================
	// 
	// ïœêî
	// 
	//==============================================================

	SupportSkillType skill_type;
	//==============================================================
	// 
	// íËêî
	// 
	//==============================================================

};