#include "magic_bullet_launcher.h"
#include "user.h"
#include "collision.h"
#include "noise.h"
//==============================================================
// 
// �R���X�g���N�^
// 
//==============================================================
MagicBulletLauncher::MagicBulletLauncher(Graphics& graphics)
{
	atk_param.power = 5;
	atk_param.invinsible_time = 0.2f;
	skill_init_param.acceleration = 50.0f;
	skill_init_param.collider_radius = 2.0f;
	max_cool_time = 0.5f;

	test_slash_hit = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/slash_ray.fbx");
	test_slash_hit->set_material(MaterialManager::instance().mat_fire_distortion.get());
	test_slash_hit->set_init_scale(2.0f);
	test_slash_hit->set_init_life_duration(0.1f);

	test_slash_hit->set_init_color({ 2.5f,2.5f,5.9f,0.5f });

}
//==============================================================
// 
// �X�V
// 
//==============================================================
void MagicBulletLauncher::update(Graphics& graphics, float elapsed_time)
{
	//�x�[�X�̃A�b�v�f�[�g
	SkillLauncher::update(graphics, elapsed_time);

	//�G�t�F�N�g�X�V
	test_slash_hit->update(graphics, elapsed_time);

}
//==============================================================
// 
// �`��
// 
//==============================================================
void MagicBulletLauncher::render(Graphics& graphics,Camera* camera)
{
	//�x�[�X�̕`��
	SkillLauncher::render(graphics,camera);

	//�G�t�F�N�g�X�V
	test_slash_hit->render(graphics, camera);

}
//==============================================================
// 
// �X�L������
// 
//==============================================================
bool MagicBulletLauncher::chant(Graphics& graphics, DirectX::XMFLOAT3* init_pos, DirectX::XMFLOAT3* dir)
{
	//�r���\�ȏ�ԂȂ�
	if (chantable)
	{
		unique_ptr<Skill> skill = make_unique<MagicBullet>(graphics, init_pos, dir, skill_init_param);
		cool_time = max_cool_time;
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
void MagicBulletLauncher::skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera)
{
	for (auto& s : skills)
	{
		if (Collision::sphere_vs_capsule(s->get_colider().start, s->get_colider().radius,
			object_colider.start, object_colider.end, object_colider.radius))
		{
			//�X�L�����q�b�g
			s->skill_hit();
			s->set_is_skill_hit(true);
			//�J�����V�F�C�N
			camera->set_camera_shake(atk_param.camera_shake);
			//�_���[�W��^����
			damaged_func(atk_param.power, atk_param.invinsible_time, WINCE_TYPE::NONE);
			//�q�b�g�G�t�F�N�g
			if (!test_slash_hit->get_active())
			{
				//�q�b�g�G�t�F�N�g
				const float middle = (s->get_colider().end.y - s->get_colider().start.y) / 2.0f;
				const DirectX::XMFLOAT3 hit_effect_pos = { s->get_colider().start.x,middle, s->get_colider().start.z };
				test_slash_hit->play(hit_effect_pos);
				test_slash_hit->set_rotate_quaternion(MeshEffect::AXIS::UP, Noise::instance().random_range(0, 90));
				test_slash_hit->set_rotate_quaternion(MeshEffect::AXIS::FORWARD, Noise::instance().random_range(0, 90));
			}
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
			ImGui::DragFloat("max_cool_time", &max_cool_time);
			ImGui::DragInt("power", &atk_param.power);
			ImGui::DragFloat("invisible_time", &atk_param.invinsible_time);
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
