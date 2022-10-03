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
	// \’ΜAρ^
	// 
	//==============================================================
	
public:
	//==============================================================
	// 
	// publicΦ
	// 
	//==============================================================
	SkillSlot(){};
	virtual ~SkillSlot() {}

	//ϊ»
	void initialize(Graphics& graphics);
	//XV
	void update(Graphics& graphics, float elapsed_time);
	//`ζ
	void render(Graphics& graphics);

	void skilland_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func);
	//DebugGUI\¦
	virtual void debug_gui(string str_id);

	//r₯·ι
	virtual void chant(Graphics& graphics) {};
	
protected:
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

	//==============================================================
	// 
	// θ
	// 
	//==============================================================

};