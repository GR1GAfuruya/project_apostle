#include "skill_spears_sea.h"
#include "noise.h"
#include "user.h"
#include "light_manager.h"
//==============================================================
// 
//�R���X�g���N�^
// 
//==============================================================
SpearsSea::SpearsSea(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos, PublicParam initparam)
{
	//�p�����[�^�[������
	initialize(graphics);


	instance_mesh = std::make_unique<InstanceMeshEffect>(graphics, "./resources/Effects/Meshes/eff_spear.fbx",MAX_NUM);
	instance_mesh->set_material(MaterialManager::instance().mat_fire_distortion.get());
	instance_mesh->constants->data.particle_color = { 1.0f,0.8f,5.5f,1.0f };
	position = launch_pos;
	target_position = target_pos;
	power = initparam.power;
	invinsible_time = initparam.invinsible_time;
	cool_time = initparam.cool_time;
	param = initparam;
	collision_type = CollisionType::SPHERE;
	//������ݒ�
	life_span = initparam.life_span;

	instance_mesh->constants->data.threshold = 0.0f;

	for (int i = 0; i < MAX_NUM; i++)
	{
		//---�������X�ɐL�΂�---//
		//�L�΂�
		instance_mesh->set_scale(0, i);
	}


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
	life_span = 0;
	finish = false;
	emit_num = 0;
	target_position = {};
	init_flag = false;
	init_emit_num = 10;
};
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

		//�e���̏o���ʒu
		DirectX::XMFLOAT3 appearance_pos{};
		instance_mesh->play(appearance_pos);
		//�ǔ����Ԃ����ɒB���邱�ƂŃt�B�j�b�V����Ԃ�
		if ( follow_timer < param.follow_time)
		{
			//�^�C�}�[��i�߂�
			follow_timer += elapsed_time;
			position = target_position;
			//�������ɖ��߂�
			position.y = target_position.y - 2.0f;
		}
		else
		{

			//�U���̓����蔻��p�����[�^�[�ݒ�
			attack_colider.start = { position.x,position.y,position.z };
			attack_colider.radius = param.radius;
			//�t�B�j�b�V�����͑�����C�ɂ��ׂďo��
			if (!finish)
			{
				spear_emit(0,MAX_NUM, SPEAR_SIZE);
				finish = true;
				//���C�g�ݒu
				DirectX::XMFLOAT3 point_light_pos = { position.x,position.y + 10.0f,position.z };//���̈ʒu��菭����ɔz�u
				spear_light = make_shared<PointLight>(graphics, point_light_pos, 30.0f, DirectX::XMFLOAT3(1.0f, 0.8f, 5.5f));
				LightManager::instance().register_light("SpearsSea", spear_light);

			}

			//���̍X�V����
			const float deley_rate = 0.1f;
			for (int i = 0; i < MAX_NUM; i++)
			{
				//---�������X�ɐL�΂�---//
				//����
				float rate = param.spear_length_rate * elapsed_time;
				//�L�΂�
				param.spear_length = lerp(instance_mesh->get_scale(i).z, SPEAR_SIZE, rate);
				instance_mesh->set_scale({ SPEAR_SIZE,SPEAR_SIZE,param.spear_length }, i);
			}


		}

	}


	const float threshold_rate = 2.0f;
	const float start_threshold_time = 0.5f;
	//�f�B�]���u����
	if(life_time > life_span - start_threshold_time)
	{
		if (instance_mesh->constants->data.threshold <= 1.0f)
		{
			instance_mesh->constants->data.threshold += threshold_rate * elapsed_time;
		}
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
//���o��
// 
//==============================================================
void SpearsSea::spear_emit(int index_offset, int emit_max_num, float size)
{
	DirectX::XMFLOAT3 appearance_pos{};
	for (int i = 0; i < emit_max_num; i++)
	{
		int random = std::abs(static_cast<int>(Noise::instance().get_rnd()));
		float circle_radius = random % static_cast<int>(param.radius);
		appearance_pos.x = Math::circumferential_placement({ position.x,position.z }, circle_radius, emit_num, MAX_NUM).x;
		appearance_pos.y = position.y - 3.0f;
		appearance_pos.z = Math::circumferential_placement({ position.x,position.z }, circle_radius, emit_num, MAX_NUM).y;
		//�΂�΂�ɐ��₷
		DirectX::XMFLOAT3 spear_dir = Math::Normalize(DirectX::XMFLOAT3(cosf(emit_num), 1.01f, sinf(emit_num)));
		instance_mesh->set_position(appearance_pos, emit_num);
		instance_mesh->set_scale({ size,size,0 }, emit_num);
		instance_mesh->set_orientation(instance_mesh->rotate_base_axis(InstanceMeshEffect::AXIS::FORWARD, spear_dir, emit_num), emit_num);
		instance_mesh->set_is_loop(true);
		emit_num++;

		if (emit_num >= MAX_NUM) emit_num = index_offset;
	}
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
	ImGui::DragFloat("threshold", &instance_mesh->constants->data.threshold,0.01f);
	ImGui::DragInt("emit_num", &emit_num,0.1f);
	ImGui::DragFloat3("pos", &position.x,0.1f);
	ImGui::DragFloat3("target_position", &target_position.x,0.1f);
	if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		instance_mesh->debug_gui(skill_name);
	}

	/*�������Ƀp�����[�^�[�L�q*/
	ImGui::PopID();
	ImGui::End();


#endif
}
