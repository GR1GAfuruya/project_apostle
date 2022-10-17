#include "restraint_launcher.h"
#include "skill_restraint.h"

RestraintLauncher::RestraintLauncher(Graphics& graphics)
{
}

bool RestraintLauncher::chant(Graphics& graphics, DirectX::XMFLOAT3* target_pos, DirectX::XMFLOAT3* down_speed)
{
	//‰r¥‰Â”\‚Èó‘Ô‚È‚ç
	if (chantable)
	{
	std:unique_ptr<Skill> skill = make_unique<Restrain>();
		//ƒŠƒXƒg‚É’Ç‰Á
		skills.push_back(std::move(skill));
		chantable = false;
		return true;
	}
	return false;
}

void RestraintLauncher::debug_gui()
{
}
