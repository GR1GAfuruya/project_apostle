#pragma once
#include "skill_slot.h"
#define AT_SKILLTYPE AttackSkillSlot::AttackSkillType

class AttackSkillSlot : public SkillSlot
{
public:
	//==============================================================
	// 
	// 構造体、列挙型
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
	// public関数
	// 
	//==============================================================
	AttackSkillSlot(Graphics& graphics, AttackSkillType type);
	~AttackSkillSlot() {}
	//スキルの発動
	void chant(Graphics& graphics);
	//スキルの実体生成
	void entity_generation_by_type(Graphics& graphics, std::unique_ptr<Skill>& coffin);
	//==============================================================
	// 
	// 変数
	// 
	//==============================================================

	AttackSkillType skill_type;
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================
};
