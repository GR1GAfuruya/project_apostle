#include "restraint_launcher.h"
#include "skill_restraint.h"
//==============================================================
// 
// �R���X�g���N�^
// 
//==============================================================
RestraintLauncher::RestraintLauncher()
{
	max_cool_time = 15.0f;
}
//==============================================================
// 
// �X�L������
// 
//==============================================================
bool RestraintLauncher::chant(DirectX::XMFLOAT3* target_pos, DirectX::XMFLOAT3* down_speed)
{
	//�r���\�ȏ�ԂȂ�
	if (chantable)
	{
		unique_ptr<Skill> skill = make_unique<Restrain>();
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
//�f�o�b�OGUI�\��
// 
//==============================================================
void RestraintLauncher::debug_gui()
{
#if USE_IMGUI
	imgui_menu_bar("Skill", "Restraint", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("Restraint");

		ImGui::End();
	}
#endif


}
