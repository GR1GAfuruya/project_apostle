#include "skill_physical_up.h"
#include "light_manager.h"
//==============================================================
// 
//�R���X�g���N�^
// 
//==============================================================
PhysicalUp::PhysicalUp(Graphics& graphics, DirectX::XMFLOAT3* launch_pos, float* run_speed, float* jump_speed)
{
	*run_speed *= magnification;
	*jump_speed *= magnification;
	effect_position.reset(launch_pos);
	enhanced_run_speed.reset(run_speed);
	enhanced_jump_speed.reset(jump_speed);
	//���C�g����
	light = make_shared<PointLight>(graphics, position, 15.0f, DirectX::XMFLOAT3(1.1f, 1.8f, 0.5f));
	LightManager::instance().register_light("PhysicalUp", light);
	initialize(graphics);
}

PhysicalUp::~PhysicalUp()
{
	*enhanced_run_speed /= magnification;
	*enhanced_jump_speed /= magnification;
	effect_position.release();
	enhanced_run_speed.release();
	enhanced_jump_speed.release();
	//���C�g����
	LightManager::instance().delete_light(light->name);


}
//==============================================================
// 
//������
// 
//==============================================================
void PhysicalUp::initialize(Graphics& graphics)
{
	cool_time = 10.0f;
	life_time = 10.0f;
}
//==============================================================
// 
//�X�V
// 
//==============================================================
void PhysicalUp::update(Graphics& graphics, float elapsed_time)
{
	life_time -= elapsed_time;
	const float offset = 11.0f;
	DirectX::XMFLOAT3 light_pos = { effect_position->x ,effect_position->y + offset ,effect_position->z };
	light->set_position(light_pos);
	if (life_time < 0)
	{
		skill_end_flag = true;
	}
}
//==============================================================
// 
//�`��
// 
//==============================================================
void PhysicalUp::render(Graphics& graphics)
{

}
//==============================================================
// 
//�f�o�b�OGUI
// 
//==============================================================
void PhysicalUp::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "PhysicalUp:" + str_id + "Param";
	ImGui::Begin("PhysicalUp");
	ImGui::PushID(str_id.c_str());
	/*�����艺�Ƀp�����[�^�[�L�q*/
	ImGui::BulletText(name.c_str());
	ImGui::DragFloat("life_time", &life_time);
	ImGui::DragFloat("enhanced_jump_speed", &(*enhanced_jump_speed));
	ImGui::DragFloat3("pos", &position.x);
	
	/*�������Ƀp�����[�^�[�L�q*/
	ImGui::PopID();
	ImGui::End();

#endif

}
