#pragma once

class Skill
{
public:
	Skill();
	~Skill();

	void initialize();
	void update();
	void render();

	float cool_time;
};