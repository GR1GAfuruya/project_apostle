#pragma once
#include "skill.h"
#include "instance_mesh_effect.h"
#include "light.h"
class SpearsSea : public Skill
{
public:
	//==============================================================
	// 
	// ç\ë¢ëÃÅAóÒãìå^
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
	// publicä÷êî
	// 
	//==============================================================
	SpearsSea(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, InitializeParam initparam);
	~SpearsSea();

	void initialize(Graphics& graphics) override;
	void update(Graphics& graphics, float elapsed_time)override;
	void render(Graphics& graphics)override;

	void debug_gui(string str_id);
private:
	//==============================================================
	// 
	// íËêî
	// 
	//==============================================================
	static constexpr int MAX_NUM = 50;
	static constexpr float SPEAR_SIZE = 0.5f;
	//static constexpr DirectX::XMFLOAT3 SPEAR_SIZE = { 0.5f,0.5f,1.0f };

	//==============================================================
	// 
	// ïœêî
	// 
	//==============================================================
	std::unique_ptr<MeshEffect> main_effect[MAX_NUM];
	std::unique_ptr<InstanceMeshEffect> instance_mesh;
	float life_span = 0;
	float radius;
	float spear_length;
	float spear_length_rate;
	std::shared_ptr<PointLight> spear_light;
	
};