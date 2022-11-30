#include "skill_launcher.h"
#include "user.h"
#include "imgui_include.h"
#include "collision.h"
//==============================================================
// 
//������
// 
//==============================================================
void SkillLauncher::initialize(Graphics& graphics)
{
	 cool_time = 0;
	//�r���\��
	 chantable = true;
	 
	 cool_time_attenuation_speed = 1.0f;
	// skill.clear();
}
//==============================================================
// 
//�X�V
// 
//==============================================================
void SkillLauncher::update(Graphics& graphics, float elapsed_time)
{
	for (auto& s : skills)
	{
		s->update(graphics, elapsed_time);
	}
	//�������s�����X�L�����폜
	skills.erase(std::remove_if(std::begin(skills), std::end(skills),
		[](std::unique_ptr<Skill> const& s) { return s->is_skill_end(); }), std::end(skills));

	//�N�[���^�C���X�V
	if (cool_time > 0)
	{
		cool_time -= cool_time_attenuation_speed * elapsed_time;
	}
	else
	{
		cool_time = 0;
		chantable = true;
	}
}


//==============================================================
// 
//�`��
// 
//==============================================================
void SkillLauncher::render(Graphics& graphics)
{
	for (auto& s : skills)
	{
		s->render(graphics);
	}
}
