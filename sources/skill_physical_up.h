#pragma once
#include "skill.h"
class PhysucalUp : public Skill
{
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	PhysucalUp() {}
	~PhysucalUp() {}

	void initialize(Graphics& graphics) override;
	void update(Graphics& graphics, float elapsed_time)override;
	void render(Graphics& graphics)override;

	bool is_skill_end() { return skill_end_flag; };

	void debug_gui(string str_id);
protected:
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

	//==============================================================
	// 
	// �萔
	// 
	//==============================================================
public:

};