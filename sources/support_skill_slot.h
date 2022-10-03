#pragma once
#include "skill_slot.h"
#define SP_SKILLTYPE SupportSkillSlot::SupportSkillType

class SupportSkillSlot : public SkillSlot
{
public:
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================
	enum class SupportSkillType
	{
		PHYCICAL_UP,
		REGENERATE,
		RESTRAINNT,
	};


	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	SupportSkillSlot(Graphics& graphics, SupportSkillType type);
	~SupportSkillSlot() {}
	void chant(Graphics& graphics);
	//==============================================================
	// 
	// private関数
	// 
	//==============================================================
	//スキルの実体生成
	void entity_generation_by_type(Graphics& graphics, std::unique_ptr<Skill>& coffin) ;
	//==============================================================
	// 
	// 変数
	// 
	//==============================================================

	SupportSkillType skill_type;
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================

};