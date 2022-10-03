#include "skill_slot.h"
#include "user.h"
#include "imgui_include.h"
#include "collision.h"
//初期化
void SkillSlot::initialize(Graphics& graphics)
{
	 cool_time = 0;
	//詠唱可能か
	 chantable = true;
	 
	 cool_time_attenuation_speed = 1.0f;
	// skill.clear();
}

//更新
void SkillSlot::update(Graphics& graphics, float elapsed_time)
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
		cool_time -= cool_time_attenuation_speed * elapsed_time;
	}

	if (cool_time <= 0)
	{
		cool_time = 0;
		chantable = true;
	}
}

//描画
void SkillSlot::render(Graphics& graphics)
{
	for (auto& s : skills)
	{
		s->render(graphics);
	}
}

void SkillSlot::skilland_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func)
{
	for (auto& s : skills)
	{
		switch (s->get_collision_type())
		{
		case Skill::CollisionType::SPHERE:
			if (Collision::sphere_vs_capsule(s->get_colider().start, s->get_colider().radius,
				object_colider.start, object_colider.end, object_colider.radius))
			{
				s->skill_hit();
				damaged_func(s->get_power(), s->get_invinsible_time());
			}
			break;
		case Skill::CollisionType::CYLINDER:

			break;
		case Skill::CollisionType::CUBE:

			break;
		case Skill::CollisionType::CAPSULE:
			if (Collision::capsule_vs_capsule(s->get_colider().start, s->get_colider().end, s->get_colider().radius,
				object_colider.start, object_colider.end, object_colider.radius))
			{
				s->skill_hit();
				damaged_func(s->get_power(), s->get_invinsible_time());
			}
			break;
		default:
			break;
		}
		

	}
}

void SkillSlot::debug_gui(string str_id)
{
#if USE_IMGUI

	string name = "Skill:" + str_id;
	string cooltime_str = str_id + ":ct";
	ImGui::Begin("Skill");
	if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat(cooltime_str.c_str(), &cool_time);
	}

	int count = 0;
	for (auto& s : skills)
	{
		s->debug_gui(to_string(count).c_str());
		count++;
	}
	ImGui::Separator();
	ImGui::End();
	
#endif

	

}

