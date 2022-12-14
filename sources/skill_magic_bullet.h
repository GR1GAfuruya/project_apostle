#pragma once
#include "skill.h"
#include "light.h"

class MagicBullet : public Skill
{
public:
	//==============================================================
	// 
	// 構造体、列挙型
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
	// public関数
	// 
	//=============================================================
	MagicBullet(Graphics& graphics, DirectX::XMFLOAT3 init_pos, DirectX::XMFLOAT3 dir, InitializeParam initparam);
	~MagicBullet();

	void initialize(Graphics& graphics) ;
	void update(Graphics& graphics, float elapsed_time)override;
	void render(Graphics& graphics)override;


	void debug_gui(string str_id);
protected:
	

	//==============================================================
	// 
	// 変数
	// 
	//==============================================================
	DirectX::XMFLOAT3 position{};
	DirectX::XMFLOAT3 velocity{};
	float acceleration;
	std::unique_ptr<MeshEffect> main_effect;
	std::shared_ptr<PointLight> spear_light;

	//==============================================================
	// 
	// 定数
	// 
	//==============================================================
public:

};