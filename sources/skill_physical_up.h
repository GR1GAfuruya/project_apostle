#pragma once
#include "skill.h"
class PhysicalUp : public Skill
{
public:
	//==============================================================
	// 
	// publicΦ
	// 
	//==============================================================
	PhysicalUp(DirectX::XMFLOAT3* launch_pos, float* run_speed, float* jump_speed);
	~PhysicalUp();

	void initialize() override;
	void update(float elapsed_time)override;
	void render(Camera* camera)override;

	void debug_gui(string str_id);
protected:
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
	//­»{¦
	float magnification = 2.0f;
	//==============================================================
	// 
	// θ
	// 
	//==============================================================
public:
	std::unique_ptr<float> enhanced_run_speed;
	std::unique_ptr<float> enhanced_jump_speed;
	std::unique_ptr<DirectX::XMFLOAT3> effect_position;
	std::shared_ptr<PointLight> light;
};