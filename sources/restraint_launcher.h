#pragma once
#include "skill_launcher.h"
//==============================================================
// 
// RestraintLauncher�N���X�@
// 
//==============================================================
class RestraintLauncher : public SkillLauncher
{
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	RestraintLauncher(Graphics& graphics);
	~RestraintLauncher() override {};
	//����
	bool chant(Graphics& graphics, DirectX::XMFLOAT3* target_pos, DirectX::XMFLOAT3* down_speed);

	//DebugGUI�\��
	void debug_gui() override;
	

};