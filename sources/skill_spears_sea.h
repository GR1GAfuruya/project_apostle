#pragma once
#include "skill.h"
class SpearsSea : public Skill
{
public:
	//==============================================================
	// 
	// \‘¢‘ÌA—ñ‹“Œ^
	// 
	//==============================================================
	struct InitializeParam
	{
		float power;
		float invinsible_time;
		float cool_time;
		float radius;
		float collider_radius;
	};
	//==============================================================
	// 
	// publicŠÖ”
	// 
	//==============================================================
	SpearsSea(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, InitializeParam initparam);
	~SpearsSea() {}

	void initialize(Graphics& graphics) override;
	void update(Graphics& graphics, float elapsed_time)override;
	void render(Graphics& graphics)override;

	void debug_gui(string str_id);
private:
	//==============================================================
	// 
	// ’è”
	// 
	//==============================================================
	static constexpr int MAX_NUM = 8;

	//==============================================================
	// 
	// •Ï”
	// 
	//==============================================================
	std::unique_ptr<MeshEffect> main_effect[MAX_NUM];
	float life_span = 0;
	float radius;
public:

};