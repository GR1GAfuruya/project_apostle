#pragma once
#include "skill.h"
#include "instance_mesh_effect.h"
class SpearsSea : public Skill
{
public:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================
	struct InitializeParam
	{
		float power;
		float invinsible_time;
		float cool_time;
		float radius;
		float collider_radius;
		DirectX::XMFLOAT2 dir{};
	};
	//==============================================================
	// 
	// public�֐�
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
	// �萔
	// 
	//==============================================================
	static constexpr int MAX_NUM = 20;

	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================
	std::unique_ptr<MeshEffect> main_effect[MAX_NUM];
	std::unique_ptr<InstanceMeshEffect> instance_mesh;
	float life_span = 0;
	float radius;
	DirectX::XMFLOAT2 dir{};
public:

};