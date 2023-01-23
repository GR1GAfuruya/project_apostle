#include "slash_wave_launcher.h"

SlashWaveLauncher::SlashWaveLauncher(Graphics& graphics)
{
}

bool SlashWaveLauncher::chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3* dir)
{
	//詠唱可能な状態なら
	if (chantable)
	{
	std:unique_ptr<Skill> skill = make_unique<SlashWave>(graphics, launch_pos, dir, init_param);
		//リストに追加
		cool_time = max_cool_time;
		skills.push_back(std::move(skill));
		chantable = false;
		return true;
	}
	return false;
}

void SlashWaveLauncher::skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera)
{
	for (auto& s : skills)
	{
		if (Collision::sphere_vs_capsule(s->get_colider().start, s->get_colider().radius,
			object_colider.start, object_colider.end, object_colider.radius))
		{
			//スキルがヒット
			s->skill_hit();
			s->set_is_skill_hit(true);
			//カメラシェイク
			camera->set_camera_shake(s->get_atk_param().camera_shake);
			//ダメージを与える
			damaged_func(s->get_atk_param().power, s->get_atk_param().invinsible_time, WINCE_TYPE::NONE);
		}
	}
}

void SlashWaveLauncher::debug_gui()
{
}
