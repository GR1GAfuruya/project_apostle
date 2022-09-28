#include "skill_magic_bullet.h"


void MagicBullet::initialize(Graphics& graphics)
{
	life_time = 5;
	cool_time = 0;
}

void MagicBullet::update(Graphics& graphics, float elapsed_time)
{
	life_time -= elapsed_time;
	if (life_time < 0) skill_end_flag = true;

}

void MagicBullet::render(Graphics& graphics)
{

}

void MagicBullet::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "MagicBullet:" + str_id + "Param";
	ImGui::Begin("Skill");
	ImGui::PushID(str_id.c_str());
	/*�����艺�Ƀp�����[�^�[�L�q*/
	ImGui::BulletText(name.c_str());
	ImGui::DragFloat("life_time", &life_time);
	

	/*�������Ƀp�����[�^�[�L�q*/
	ImGui::PopID();
	ImGui::End();

#endif

}
