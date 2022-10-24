#pragma once
#include "skill.h"

class MagicBullet : public Skill
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
		float invisible_time;
		float cool_time;
		float acceleration;
		float collider_radius;
	};
	//==============================================================
	// 
	// publicä÷êî
	// 
	//==============================================================
	MagicBullet(){}
	MagicBullet(Graphics& graphics, DirectX::XMFLOAT3 init_pos, DirectX::XMFLOAT3 dir, InitializeParam initparam);
	~MagicBullet() {}

	void initialize(Graphics& graphics) ;
	void update(Graphics& graphics, float elapsed_time)override;
	void render(Graphics& graphics)override;


	void debug_gui(string str_id);
protected:
	

	//==============================================================
	// 
	// ïœêî
	// 
	//==============================================================
	DirectX::XMFLOAT3 position{};
	DirectX::XMFLOAT3 velocity{};
	float acceleration;
	std::unique_ptr<MeshEffect> main_effect;
	//==============================================================
	// 
	// íËêî
	// 
	//==============================================================
public:

};