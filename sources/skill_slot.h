#pragma once
#include <vector>
#include "sprite.h"
#include "skill.h"
#include "damage_func.h"

class SkillSlot
{
public:
	//==============================================================
	// 
	// \‘¢‘ÌA—ñ‹“Œ^
	// 
	//==============================================================
	
public:
	//==============================================================
	// 
	// publicŠÖ”
	// 
	//==============================================================
	SkillSlot(){};
	virtual ~SkillSlot() {}

	//‰Šú‰»
	void initialize(Graphics& graphics);
	//XV
	void update(Graphics& graphics, float elapsed_time);
	//•`‰æ
	void render(Graphics& graphics);

	void skilland_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func);
	//DebugGUI•\¦
	virtual void debug_gui(string str_id);

	//‰r¥‚·‚é
	virtual void chant(Graphics& graphics) {};
	
protected:
	//==============================================================
	// 
	// •Ï”
	// 
	//==============================================================
	float cool_time;
	//‰r¥‰Â”\‚©
	bool chantable;

	float cool_time_attenuation_speed;
	std::vector<std::unique_ptr<Skill>> skills;

	//==============================================================
	// 
	// ’è”
	// 
	//==============================================================

};