#pragma once
#include <vector>
#include "sprite.h"
#include "skill.h"
class SkillSlot
{
public:
	//==============================================================
	// 
	// publicä÷êî
	// 
	//==============================================================
	SkillSlot() {};
	SkillSlot(Graphics& graphics) {};
	~SkillSlot() {}

	void initialize(Graphics& graphics);
	void update(Graphics& graphics, float elapsed_time);
	void render(Graphics& graphics);
	void debug_gui();

	//ârè•Ç∑ÇÈ
	void chant(Graphics& graphics);
	//==============================================================
	// 
	// ç\ë¢ëÃÅAóÒãìå^
	// 
	//==============================================================



	//==============================================================
	// 
	// ïœêî
	// 
	//==============================================================
	float cool_time;
	//ârè•â¬î\Ç©
	bool chantable;

	float cool_time_attenuation_speed;
	std::unique_ptr<Sprite> icon;
	std::vector<std::unique_ptr<Skill>> skill;

	//==============================================================
	// 
	// íËêî
	// 
	//==============================================================

};