#pragma once
#include "skill.h"
#include "instance_mesh_effect.h"
#include "light.h"
class LightningRain : public Skill
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
		//雷の長さ
		float lightning_length;

	};
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	LightningRain(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos, PublicParam initparam);
	~LightningRain();

	void initialize(Graphics& graphics) override;
	void update(Graphics& graphics, float elapsed_time)override;
	void render(Graphics& graphics, Camera* camera)override;

	void lightning_emit(int index_offset, int emit_max_num, float size);
	void debug_gui(string str_id);
private:
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================
	static constexpr int MAX_NUM = 60;
	static constexpr float SPEAR_SIZE = 0.6f;
	static constexpr float FOLLOW_TIME = 0.7f;
	//static constexpr DirectX::XMFLOAT3 SPEAR_SIZE = { 0.5f,0.5f,1.0f };

	//==============================================================
	// 
	// 変数
	// 
	//==============================================================
	//インスタンシングメッシュ
	std::unique_ptr<MeshEffect> lightning_mesh;
	//ポイントライト
	std::shared_ptr<PointLight>lightning_light;

	PublicParam param;
	//槍を出した数
	int emit_num;
	//フィニッシュしたかのフラグ
	bool finish;
	//追従する時間
	float follow_timer;

	//ターゲットの位置
	DirectX::XMFLOAT3 target_position;
	//槍出現時最初のフレームのみ行う処理に使う変数
	bool init_flag;
	int init_emit_num;

};
