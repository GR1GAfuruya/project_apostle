#pragma once
#include "skill.h"
class MagicBullet : public Skill
{
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	MagicBullet() {}
	~MagicBullet() {}

	void initialize(Graphics& graphics) override;
	void initialize(Graphics& graphics, DirectX::XMFLOAT3 init_pos, DirectX::XMFLOAT3 target_pos)override;
	void update(Graphics& graphics, float elapsed_time)override;
	void render(Graphics& graphics)override;


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
	DirectX::XMFLOAT3 position{};
	DirectX::XMFLOAT3 velocity{};
	float acceleration;

	//==============================================================
	// 
	// 定数
	// 
	//==============================================================
public:

};