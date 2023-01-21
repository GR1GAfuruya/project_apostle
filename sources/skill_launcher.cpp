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
	 //�N�[���^�C���Z�k���̐ݒ�@�����̓N�[���^�C���̍ő�l��1��
	 reduction_rate = 0.1f;
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
		cool_time -=  elapsed_time;
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
//==============================================================
// 
//�N�[���^�C���Z�k
// 
//==============================================================
void SkillLauncher::cool_time_reduction()
{
	
	if (cool_time > 0)
	{
		//�Z�k���͌X�Őݒ�
		cool_time -= max_cool_time * reduction_rate;
		//0�ȉ��ɂȂ�����0�ɖ߂��Ă���
		if (cool_time < 0) cool_time = 0;
	}
	
}
