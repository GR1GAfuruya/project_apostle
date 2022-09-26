#pragma once
class SkillSlot
{
public:
	SkillSlot();
	~SkillSlot();

	void initialize();
	void update();
	void render();

	float cool_time;
};