#include "skill_spears_sea.h"
#include "noise.h"
#include "user.h"
#include "light_manager.h"
//==============================================================
// 
//�R���X�g���N�^
// 
//==============================================================
SpearsSea::SpearsSea(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, InitializeParam initparam)
{

	instance_mesh = std::make_unique<InstanceMeshEffect>(graphics, "./resources/Effects/Meshes/eff_spear.fbx",MAX_NUM);
	instance_mesh->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill2_output.png");
	instance_mesh->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/T_Perlin_Noise_M.tga");
	instance_mesh->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga");
	instance_mesh->create_pixel_shader(graphics.get_device().Get(), "./shaders/fire_distortion.cso");
	instance_mesh->constants->data.particle_color = { 1.0f,0.8f,5.5f,1.0f };
	position = launch_pos;
	power = initparam.power;
	invinsible_time = initparam.invinsible_time;
	cool_time = initparam.cool_time;
	radius = initparam.radius;
	
	collision_type = CollisionType::SPHERE;
	//������ݒ�
	life_span = 3.0f;
	spear_length = 0;
	spear_length_rate = 8.0f;
}
//==============================================================
// 
//�f�X�g���N�^
// 
//==============================================================
SpearsSea::~SpearsSea()
{
	LightManager::instance().delete_light(spear_light->name);
}
//==============================================================
// 
//������
// 
//==============================================================
void SpearsSea::initialize(Graphics& graphics)
{
}
//==============================================================
// 
//�X�V����
// 
//==============================================================
void SpearsSea::update(Graphics& graphics, float elapsed_time)
{
	life_time += elapsed_time;
	//���G�t�F�N�g�̍Đ�
	if (life_time > 0.5f)
	{
		//���łɍĐ���ԂȂ�X�V���Ȃ�
		if (!instance_mesh->get_active())
		{
			//�e���̏o���ʒu
			DirectX::XMFLOAT3 appearance_pos{};
			for (int i = 0; i < MAX_NUM; i++)
			{
				instance_mesh->play(appearance_pos);

				int random = std::abs(static_cast<int>(Noise::instance().get_rnd()));
				float circle_radius = random % static_cast<int>(radius);
				appearance_pos.x = Math::circumferential_placement({ position.x,position.z }, circle_radius, i, MAX_NUM).x;
				appearance_pos.y = position.y;
				appearance_pos.z = Math::circumferential_placement({ position.x,position.z }, circle_radius, i, MAX_NUM).y;
				//�΂�΂�ɐ��₷
				DirectX::XMFLOAT3 spear_dir = Math::Normalize(DirectX::XMFLOAT3(cosf(i), 1.01f, sinf(i)));
				instance_mesh->set_position(appearance_pos, i);
				instance_mesh->set_scale({ SPEAR_SIZE,SPEAR_SIZE,0 }, i);
				instance_mesh->set_orientation(instance_mesh->rotate_base_axis(InstanceMeshEffect::AXIS::FORWARD, spear_dir, i), i);
				instance_mesh->set_is_loop(true);
			}

			//���C�g�ݒu
			DirectX::XMFLOAT3 point_light_pos = { position.x,position.y + 10.0f,position.z };//���̈ʒu��菭����ɔz�u
			spear_light = make_shared<PointLight>(graphics, point_light_pos, 30.0f, 1.0f, 0.8f, 5.5f);
			LightManager::instance().register_light("SpearsSea", spear_light);

			//�U���̓����蔻��p�����[�^�[�ݒ�
			attack_colider.start = { position.x,appearance_pos.y,position.z };
			attack_colider.radius = radius;
		}
	}

	//���̍X�V����
	const float deley_rate = 0.1f;
	for (int i = 0; i < MAX_NUM; i++)
	{
		//---�������X�ɐL�΂�---//
		//����
		float rate = 7.0f * elapsed_time ;
		//�L�΂�
		spear_length = lerp(instance_mesh->get_scale(i).z, SPEAR_SIZE, rate);
		instance_mesh->set_scale({ SPEAR_SIZE,SPEAR_SIZE,spear_length }, i);
	}

	


	if (life_time > life_span) skill_end_flag = true;
}
//==============================================================
// 
//�`��
// 
//==============================================================
void SpearsSea::render(Graphics& graphics)
{
	instance_mesh->render(graphics);
}
//==============================================================
// 
//�f�o�b�OGUI
// 
//==============================================================
void SpearsSea::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "SpearsSea:" + str_id + "Param";
	string skill_name = "SpearsSea";
	ImGui::Begin(skill_name.c_str());

	ImGui::PushID(str_id.c_str());
	/*�����艺�Ƀp�����[�^�[�L�q*/
	//ImGui::BulletText(name.c_str());
	ImGui::DragFloat("life_time", &life_time);
	ImGui::DragFloat("spear_length", &spear_length,0.1f);
	ImGui::DragFloat("spear_length_rate", &spear_length_rate,0.1f);
	if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		instance_mesh->debug_gui(skill_name);
	}

	/*�������Ƀp�����[�^�[�L�q*/
	ImGui::PopID();
	ImGui::End();


#endif
}
