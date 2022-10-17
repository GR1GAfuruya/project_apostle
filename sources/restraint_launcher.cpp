#include "restraint_launcher.h"
#include "skill_restraint.h"

RestraintLauncher::RestraintLauncher(Graphics& graphics)
{
}

bool RestraintLauncher::chant(Graphics& graphics, DirectX::XMFLOAT3* target_pos, DirectX::XMFLOAT3* down_speed)
{
	//詠唱可能な状態なら
	if (chantable)
	{
	std:unique_ptr<Skill> skill = make_unique<Restrain>();
		//リストに追加
		skills.push_back(std::move(skill));
		chantable = false;
		return true;
	}
	return false;
}

void RestraintLauncher::debug_gui()
{
}
