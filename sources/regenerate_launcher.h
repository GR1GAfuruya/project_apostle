#pragma once
#include "skill_launcher.h"
//==============================================================
// 
// RegenerateLauncher�N���X�@
// 
//==============================================================
class RegenerateLauncher : public SkillLauncher
{
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	RegenerateLauncher(Graphics& graphics);
	~RegenerateLauncher() override {};
	//����
	bool chant(Graphics& graphics, DirectX::XMFLOAT3* launch_pos, int* health, const int max_health);

	//DebugGUI�\��
	void debug_gui() override;


};