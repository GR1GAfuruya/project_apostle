#pragma once
#include "skill.h"

class Regenerate : public Skill
{
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	Regenerate(DirectX::XMFLOAT3* launch_pos, int* health, const int max_health);
	~Regenerate();

	void initialize() override;
	void update(float elapsed_time)override;
	void render(Camera* camera)override;

	bool is_skill_end() { return skill_end_flag; };

	void debug_gui(string str_id);
protected:
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================


	//==============================================================
	// 
	// 変数
	// 
	//==============================================================
	//回復させる対象の体力のポインタ
	std::unique_ptr<int> charactor_health;
	//ライト
	std::shared_ptr<PointLight> light;
	//ライト位置
	std::unique_ptr<DirectX::XMFLOAT3> light_position;
	//体力の最大値
	int recovery_max_health;
	//回復周期
	float recovery_timer;
	//回復量
	int recovery_rate;
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================
private:
	static constexpr float RECOVERY_SPAN = 1.0f;
};