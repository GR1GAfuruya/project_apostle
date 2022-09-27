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
	SkillSlot(Graphics& graphics) {};
	~SkillSlot() {}

	//ϊ»
	void initialize(Graphics& graphics);
	//XV
	void update(Graphics& graphics, float elapsed_time);
	//`ζ
	void render(Graphics& graphics);
	//DebugGUI\¦
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