#include "skill_regenerate.h"
#include "light_manager.h"
//==============================================================
// 
//�R���X�g���N�^
// 
//==============================================================
Regenerate::Regenerate(Graphics& graphics, DirectX::XMFLOAT3* launch_pos, int* health, const int max_health)
{ 
	light_position.reset(launch_pos);
	charactor_health.reset(health);
	recovery_max_health = max_health;

	//���C�g����
	light = make_shared<PointLight>(graphics, position, 15.0f, DirectX::XMFLOAT3(0.4f, 1.8f, 0.5f));
	LightManager::instance().register_light("Regenerate", light);

	initialize(graphics);
}
//==============================================================
// 
//�f�X�g���N�^
// 
//==============================================================
Regenerate::~Regenerate()
{
	light_position.release();
	charactor_health.release();
	//���C�g����
	LightManager::instance().delete_light(light->name);
}
//==============================================================
// 
//������
// 
//==============================================================
void Regenerate::initialize(Graphics& graphics)
{
	//�X�L������
	life_time = 6.0f;
	//�񕜎���
	recovery_timer = RECOVERY_SPAN;
	//�񕜗�
	recovery_rate = 5;
	//�N�[���^�C��
	cool_time = 9.0f;
}
//==============================================================
// 
//�X�V
// 
//==============================================================
void Regenerate::update(Graphics& graphics, float elapsed_time)
{
	//�̗͂��ő�l�ȉ��Ȃ��莞�Ԃ��Ƃɉ�
	if (*charactor_health < recovery_max_health)
	{
		recovery_timer -= elapsed_time;
		if (recovery_timer <= 0.0f)
		{
			*charactor_health += recovery_rate;
			recovery_timer = RECOVERY_SPAN;
		}
	}
	//���C�g�̈ʒu�X�V
	const float offset = 11.0f;
	DirectX::XMFLOAT3 light_pos = { light_position->x ,light_position->y + offset ,light_position->z };
	light->set_position(light_pos);
	//�X�L�����Ԃ̍X�V
	life_time -= elapsed_time;
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
void Regenerate::render(Graphics& graphics)
{
}
//==============================================================
// 
//�f�o�b�OGUI
// 
//==============================================================
void Regenerate::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "Regenerate:" + str_id + "Param";
	ImGui::Begin("Regenerate");
	ImGui::PushID(str_id.c_str());
	/*�����艺�Ƀp�����[�^�[�L�q*/
	ImGui::BulletText(name.c_str());

	ImGui::DragFloat("life_time", &life_time);
	ImGui::Checkbox("skill_end_flag", &skill_end_flag);
	
	/*�������Ƀp�����[�^�[�L�q*/
	ImGui::PopID();
	ImGui::End();

#endif

}
