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
	RegenerateLauncher(Graphics& graphics);
	~RegenerateLauncher() override {};
	//発動
	bool chant(Graphics& graphics, DirectX::XMFLOAT3* launch_pos, int* health, const int max_health);

	//DebugGUI表示
	void debug_gui() override;


};