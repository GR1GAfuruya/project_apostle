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
	RestraintLauncher(Graphics& graphics);
	~RestraintLauncher() override {};
	//発動
	bool chant(Graphics& graphics, DirectX::XMFLOAT3* target_pos, DirectX::XMFLOAT3* down_speed);

	//DebugGUI表示
	void debug_gui() override;
	

};