#pragma once
#include <vector>
#include "sprite.h"
#include "skill.h"
#include "damage_func.h"

class SkillSlot
{
public:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================
	
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	SkillSlot() :
		cool_time(0.0f),
		chantable(false),
		cool_time_attenuation_speed(1.0f) {};
	virtual ~SkillSlot() {}

	//������
	void initialize(Graphics& graphics);
	//�X�V
	void update(Graphics& graphics, float elapsed_time);
	//�`��
	void render(Graphics& graphics);

	//�X�L���̓����蔻��
	void skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func);
	//DebugGUI�\��
	virtual void debug_gui(string str_id);

	//�r������
	virtual void chant(Graphics& graphics) {};
	
protected:
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

	//==============================================================
	// 
	// �萔
	// 
	//==============================================================

};