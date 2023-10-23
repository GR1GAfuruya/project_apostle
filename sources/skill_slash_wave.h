#pragma once
#include "skill.h"
#include "instance_mesh_effect.h"
#include "light.h"
class SlashWave : public Skill
{
public:
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================
	struct PublicParam
	{
		//生存時間
		float skill_duration;
		//半径
		float radius;
		//当たり判定の半径
		float collider_radius;
		//ターゲットに向かうスピード
		float speed;
	};
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	SlashWave(DirectX::XMFLOAT3* launch_pos, DirectX::XMFLOAT3* target_dir, PublicParam initparam);
	~SlashWave();

	void initialize() override;
	void update(float elapsed_time)override;
	void render(Camera* camera)override;

	void debug_gui(string str_id);
private:
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================

	//==============================================================
	// 
	// 変数
	// 
	//==============================================================
	//インスタンシングメッシュ
	std::unique_ptr<MeshEffect> slash_mesh;
	//ポイントライト
	std::shared_ptr<PointLight> slash_light;

	PublicParam param;

	//ターゲットの位置
	std::unique_ptr<DirectX::XMFLOAT3> launch_position;
	std::unique_ptr<DirectX::XMFLOAT3> target_direction;


};
