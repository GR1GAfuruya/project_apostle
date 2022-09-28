#pragma once
#include <vector>
#include "sprite.h"
#include "skill.h"
#define SKILLTYPE SkillSlot::SkillType

class SkillSlot
{
public:
	//==============================================================
	// 
	// \’ΜAρ^
	// 
	//==============================================================
	enum class SkillType
	{
		REGENERATE,
		PHYCICAL_UP,
		RESTRAINNT,
		MAGICBULLET,
		SPEARS_SEA
	};

public:
	//==============================================================
	// 
	// publicΦ
	// 
	//==============================================================
	SkillSlot(Graphics& graphics, SkillType type);
	~SkillSlot() {}

	//ϊ»
	void initialize(Graphics& graphics);
	//XV
	void update(Graphics& graphics, float elapsed_time);
	//`ζ
	void render(Graphics& graphics);
	//DebugGUI\¦
	void debug_gui(string str_id);

	//r₯·ι
	void chant(Graphics& graphics);
	

	void entity_generation_by_type(Graphics& graphics, std::unique_ptr<Skill>& coffin ,SkillType type);
	//==============================================================
	// 
	// Ο
	// 
	//==============================================================
	float cool_time;
	//r₯Β\©
	bool chantable;

	float cool_time_attenuation_speed;
	std::vector<std::unique_ptr<Skill>> skills;

	SkillType skill_type;
	//==============================================================
	// 
	// θ
	// 
	//==============================================================

};