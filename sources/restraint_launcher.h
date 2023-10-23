#pragma once
#include "skill_launcher.h"
//==============================================================
// 
// RestraintLauncherクラス　
// 
//==============================================================
class RestraintLauncher : public SkillLauncher
{
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	RestraintLauncher();
	~RestraintLauncher() override {};
	//発動
	bool chant(DirectX::XMFLOAT3* target_pos, DirectX::XMFLOAT3* down_speed);

	//DebugGUI表示
	void debug_gui() override;


};