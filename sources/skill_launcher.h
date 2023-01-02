#pragma once
#include <vector>
#include "sprite.h"
#include "skill.h"
#include "damage_func.h"

class SkillLauncher
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
	SkillLauncher() :
		cool_time(0.0f),
		chantable(false),
		reduction_rate(1.0f) {};
	virtual ~SkillLauncher() {}

	//������
	virtual void initialize(Graphics& graphics);
	//�X�V
	virtual void update(Graphics& graphics, float elapsed_time);
	//�`��
	virtual void render(Graphics& graphics);
	//�X�L���N�[���^�C���Z�k
	void cool_time_reduction();
	//DebugGUI�\��
	virtual void debug_gui()= 0;
	//�N�[���^�C���擾
	float get_cool_time() { return cool_time ; }
	//�N�[���^�C���̃p�[�Z���g�擾
	float get_cool_time_percent() { return cool_time <= 0 ? 0.0f :1.0f - ( cool_time / max_cool_time); }

	//�����������ꂼ��قȂ�̂Ōp����Ŏ���
	////�X�L���̓����蔻��
	//void skill_object_hit_judgment(Capsule object_colider, AddDamageFunc damaged_func);
	////�r������
	//void launch();
	
	
protected:
	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================
	float cool_time = 0;
	float max_cool_time = 0;
	//�r���\��
	bool chantable;
	//�N�[���^�C���Z�k��
	float reduction_rate;
	std::vector<std::unique_ptr<Skill>> skills;

	bool display_imgui = false;
	//==============================================================
	// 
	// �萔
	// 
	//==============================================================

};