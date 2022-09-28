#include "skill_slot.h"
#include "user.h"
#include "imgui_include.h"
#include "skill_physical_up.h"
#include "skill_regenerate.h"
#include "skill_restraint.h"
#include "skill_magic_bullet.h"
#include "skill_spears_sea.h"
SkillSlot::SkillSlot(Graphics& graphics, SkillType type)
{
	initialize(graphics);
	skill_type = type;
}

//������
void SkillSlot::initialize(Graphics& graphics)
{
	 cool_time = 0;
	//�r���\��
	 chantable = true;
	 
	 cool_time_attenuation_speed = 1.0f;
	// skill.clear();
}

//�X�V
void SkillSlot::update(Graphics& graphics, float elapsed_time)
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

	if (cool_time <= 0)
	{
		cool_time = 0;
		chantable = true;
	}
}

//�`��
void SkillSlot::render(Graphics& graphics)
{
	for (auto& s : skills)
	{
		s->render(graphics);
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


//�r��
void SkillSlot::chant(Graphics& graphics)
{
	//�r���\�ȏ�ԂȂ�
	if (chantable)
	{
		std::unique_ptr<Skill>skill;
		//���Ԑ���
		entity_generation_by_type(graphics, skill, skill_type);
		//�N�[���^�C���ݒ�
		skill->initialize(graphics);
		cool_time = skill->get_cool_time();
		//���X�g�ɒǉ�
		skills.push_back(std::move(skill));
		chantable = false;
	}
}

void SkillSlot::entity_generation_by_type(Graphics& graphics, std::unique_ptr<Skill>& coffin, SkillType type)
{
	switch (type)
	{
	case SkillSlot::SkillType::REGENERATE:
		coffin = std::make_unique<Regenerate>();
		break;
	case SkillSlot::SkillType::PHYCICAL_UP:
		coffin = std::make_unique<PhysucalUp>();
		break;
	case SkillSlot::SkillType::RESTRAINNT:
		coffin = std::make_unique<Restrain>();
		break;
	case SkillSlot::SkillType::MAGICBULLET:
		coffin = std::make_unique<MagicBullet>();
		break;
	case SkillSlot::SkillType::SPEARS_SEA:
		coffin = std::make_unique<SpearsSea>();

		break;
	default:
		break;
	}
}
