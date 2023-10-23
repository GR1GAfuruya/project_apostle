#pragma once
#include "skill_launcher.h"
//==============================================================
// 
// RegenerateLauncherクラス　
// 
//==============================================================
class RegenerateLauncher : public SkillLauncher
{
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	RegenerateLauncher();
	~RegenerateLauncher() override {};
	//発動
	bool chant(DirectX::XMFLOAT3* launch_pos, int* health, const int max_health);

	//DebugGUI表示
	void debug_gui() override;


};