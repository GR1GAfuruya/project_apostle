#pragma once
#include <vector>
#include "sprite.h"
#include "skill.h"
class SkillSlot
{
public:
	//==============================================================
	// 
	// publicΦ
	// 
	//==============================================================
	SkillSlot() {};
	SkillSlot(Graphics& graphics) {};
	~SkillSlot() {}

	void initialize(Graphics& graphics);
	void update(Graphics& graphics, float elapsed_time);
	void render(Graphics& graphics);
	void debug_gui();

	//r₯·ι
	void chant(Graphics& graphics);
	//==============================================================
	// 
	// \’ΜAρ^
	// 
	//==============================================================



	//==============================================================
	// 
	// Ο
	// 
	//==============================================================
	float cool_time;
	//r₯Β\©
	bool chantable;

	float cool_time_attenuation_speed;
	std::unique_ptr<Sprite> icon;
	std::vector<std::unique_ptr<Skill>> skill;

	//==============================================================
	// 
	// θ
	// 
	//==============================================================

};