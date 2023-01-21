#include "skill_launcher.h"
#include "user.h"
#include "imgui_include.h"
#include "collision.h"
//==============================================================
// 
//初期化
// 
//==============================================================
void SkillLauncher::initialize(Graphics& graphics)
{
	 cool_time = 0;
	//詠唱可能か
	 chantable = true;
	 //クールタイム短縮率の設定　初期はクールタイムの最大値の1割
	 reduction_rate = 0.1f;
	// skill.clear();
}
//==============================================================
// 
//更新
// 
//==============================================================
void SkillLauncher::update(Graphics& graphics, float elapsed_time)
{
	for (auto& s : skills)
	{
		s->update(graphics, elapsed_time);
	}
	//寿命が尽きたスキルを削除
	skills.erase(std::remove_if(std::begin(skills), std::end(skills),
		[](std::unique_ptr<Skill> const& s) { return s->is_skill_end(); }), std::end(skills));

	//クールタイム更新
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
//描画
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
//クールタイム短縮
// 
//==============================================================
void SkillLauncher::cool_time_reduction()
{
	
	if (cool_time > 0)
	{
		//短縮率は個々で設定
		cool_time -= max_cool_time * reduction_rate;
		//0以下になったら0に戻しておく
		if (cool_time < 0) cool_time = 0;
	}
	
}
