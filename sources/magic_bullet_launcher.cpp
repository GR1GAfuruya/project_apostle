#include "magic_bullet_launcher.h"
#include "user.h"
#include "collision.h"
//==============================================================
// 
// �R���X�g���N�^
// 
//==============================================================
MagicBulletLauncher::MagicBulletLauncher(Graphics& graphics)
{
	skill_init_param.power = 5;
	skill_init_param.invisible_time = 0.2f;
	skill_init_param.acceleration = 50.0f;
	skill_init_param.collider_radius = 2.0f;
	skill_init_param.cool_time = 0.5f;
}
//==============================================================
// 
// �X�L������
// 
//==============================================================
bool MagicBulletLauncher::chant(Graphics& graphics, DirectX::XMFLOAT3 init_pos, DirectX::XMFLOAT3 dir)
{
	//�r���\�ȏ�ԂȂ�
	if (chantable)
	{
		unique_ptr<Skill> skill = make_unique<MagicBullet>(graphics, init_pos, dir, skill_init_param);
		cool_time = skill->get_cool_time();

		//���X�g�ɒǉ�
		skills.push_back(std::move(skill));
		chantable = false;
		return true;
	}
	return false;
}
//==============================================================
// 
//�X�L���̓����蔻��
// 
//==============================================================
void MagicBulletLauncher::skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func)
{
	for (auto& s : skills)
	{
		if (Collision::sphere_vs_capsule(s->get_colider().start, s->get_colider().radius,
			object_colider.start, object_colider.end, object_colider.radius))
		{
			s->skill_hit();
			s->set_is_skill_hit(true);
			damaged_func(s->get_power(), s->get_invinsible_time(),WINCE_TYPE::SMALL);
		}
	}
}

//==============================================================
// 
//�f�o�b�OGUI�\��
// 
//==============================================================
void MagicBulletLauncher::debug_gui()
{
#if USE_IMGUI
	imgui_menu_bar("Skill", "MagicBullet", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("MagicBullet");
		if (ImGui::CollapsingHeader("MagicBulletLauncher", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat("cool_time", &cool_time);
			ImGui::DragFloat("init_cool_time", &skill_init_param.cool_time);
			ImGui::DragFloat("power", &skill_init_param.power);
			ImGui::DragFloat("invisible_time", &skill_init_param.invisible_time);
			ImGui::DragFloat("acceleration", &skill_init_param.acceleration);
			ImGui::DragFloat("collider_radius", &skill_init_param.collider_radius);
			int count = 0;
			for (auto& s : skills)
			{
				s->debug_gui(to_string(count).c_str());
				count++;
				ImGui::Separator();
			}


		}
		ImGui::End();
	}

#endif
}
