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
	struct PublicParam
	{
		float acceleration;
		float collider_radius;
	};
	//==============================================================
	// 
	// public関数
	// 
	//=============================================================
	MagicBullet(DirectX::XMFLOAT3* init_pos, DirectX::XMFLOAT3* dir, PublicParam initparam);
	~MagicBullet();

	void initialize();
	void update(float elapsed_time)override;
	void render(Camera* camera)override;


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
	std::unique_ptr<DirectX::XMFLOAT3> launch_pos;
	std::unique_ptr<DirectX::XMFLOAT3> target_dir;
	std::unique_ptr<MeshEffect> main_effect;
	std::unique_ptr<MeshEffect> lightning_effect[2];
	std::unique_ptr<MeshEffect> lightning_disk_effect;
	std::shared_ptr<PointLight> spear_light;

	bool launch_bullet = false;

	//==============================================================
	// 
	// 定数
	// 
	//==============================================================
public:

};