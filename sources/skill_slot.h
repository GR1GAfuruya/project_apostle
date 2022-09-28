#pragma once
#include <vector>
#include "sprite.h"
#include "skill.h"
#define SKILLTYPE SkillSlot::SkillType

class SkillSlot
{
public:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================
	enum class SkillType
	{
		REGENERATE,
		PHYCICAL_UP,
		RESTRAINNT,
		MAGICBULLET,
		SPEARS_SEA
	};

public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	SkillSlot(Graphics& graphics, SkillType type);
	~SkillSlot() {}

	//������
	void initialize(Graphics& graphics);
	//�X�V
	void update(Graphics& graphics, float elapsed_time);
	//�`��
	void render(Graphics& graphics);
	//DebugGUI�\��
	void debug_gui(string str_id);

	//�r������
	void chant(Graphics& graphics);
	

	void entity_generation_by_type(Graphics& graphics, std::unique_ptr<Skill>& coffin ,SkillType type);
	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================
	float cool_time;
	//�r���\��
	bool chantable;

	float cool_time_attenuation_speed;
	std::vector<std::unique_ptr<Skill>> skills;

	SkillType skill_type;
	//==============================================================
	// 
	// �萔
	// 
	//==============================================================

};