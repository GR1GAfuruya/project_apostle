#include "lightning_rain_launcher.h"
#include "collision.h"
//==============================================================
// 
// �R���X�g���N�^
// 
//==============================================================
LightningRainLauncher::LightningRainLauncher(Graphics& graphics)
{
	atk_param.power = 20;
	atk_param.invinsible_time = 1.0f;
	init_param.radius = 15;
	init_param.collider_radius = 8;
	init_param.lightning_length = 60.0f;
	init_param.skill_duration = 1.5f;

	//�N�[���^�C��
	max_cool_time = 15.0f;

}
//==============================================================
// 
// ����
// 
//==============================================================
bool LightningRainLauncher::chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos)
{
	//�r���\�ȏ�ԂȂ�
	if (chantable)
	{
	unique_ptr<Skill> skill = make_unique<LightningRain>(graphics, launch_pos, target_pos, init_param);
		//���X�g�ɒǉ�
		cool_time = max_cool_time;
		skills.push_back(std::move(skill));
		chantable = false;
		return true;
	}
	return false;
}
//==============================================================
// 
// �����蔻��
// 
//==============================================================
void LightningRainLauncher::skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera)
{
	for (auto& s : skills)
	{
		if (Collision::capsule_vs_capsule(s->get_colider().start, s->get_colider().end, s->get_colider().radius,
			object_colider.start, object_colider.end, object_colider.radius))
		{
			//�X�L�����q�b�g
			s->skill_hit();
			s->set_is_skill_hit(true);
			//�J�����V�F�C�N
			camera->set_camera_shake(atk_param.camera_shake);
			//�_���[�W��^����
			damaged_func(atk_param.power, atk_param.invinsible_time, WINCE_TYPE::NONE);
		}
	}
}
//==============================================================
// 
// �f�o�b�OGUI
// 
//==============================================================
void LightningRainLauncher::debug_gui()
{
#if USE_IMGUI
	imgui_menu_bar("Skill", "LightningRain", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("LightningRain");
		if (ImGui::CollapsingHeader("LightningRainLauncher", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat("cool_time", &cool_time);
			ImGui::DragFloat("max_cool_time", &max_cool_time);
			ImGui::DragFloat("life_span", &init_param.skill_duration);
			ImGui::DragInt("power", &atk_param.power);
			ImGui::DragFloat("invinsible_time", &atk_param.invinsible_time);
			ImGui::DragFloat("radius", &init_param.radius);
			ImGui::DragFloat("collider_radius", &init_param.collider_radius);
			ImGui::DragFloat("lightning_length", &init_param.lightning_length);
			ImGui::Separator();
			int count = 0;
			for (auto& s : skills)
			{
				s->debug_gui(to_string(count).c_str());
				count++;
			}
		}
		ImGui::End();
	}
#endif

}
