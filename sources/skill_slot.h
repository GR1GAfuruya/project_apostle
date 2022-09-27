#pragma once
#include <vector>
#include "sprite.h"
#include "skill.h"
class SkillSlot
{
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	SkillSlot(Graphics& graphics) {};
	~SkillSlot() {}

	//������
	void initialize(Graphics& graphics);
	//�X�V
	void update(Graphics& graphics, float elapsed_time);
	//�`��
	void render(Graphics& graphics);
	//DebugGUI�\��
	void debug_gui();

	//�r������
	void chant(Graphics& graphics);
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================



	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================
	float cool_time;
	//�r���\��
	bool chantable;

	float cool_time_attenuation_speed;
	std::unique_ptr<Sprite> icon;
	std::vector<std::unique_ptr<Skill>> skill;

	//==============================================================
	// 
	// �萔
	// 
	//==============================================================

};