#pragma once
#include "skill_slot.h"
#define SP_SKILLTYPE SupportSkillSlot::SupportSkillType

class SupportSkillSlot : public SkillSlot
{
public:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================
	enum class SupportSkillType
	{
		PHYCICAL_UP,
		REGENERATE,
		RESTRAINNT,
	};


	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	SupportSkillSlot(Graphics& graphics, SupportSkillType type);
	~SupportSkillSlot() {}
	void chant(Graphics& graphics);
	//==============================================================
	// 
	// private�֐�
	// 
	//==============================================================
	//�X�L���̎��̐���
	void entity_generation_by_type(Graphics& graphics, std::unique_ptr<Skill>& coffin) ;
	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================

	SupportSkillType skill_type;
	//==============================================================
	// 
	// �萔
	// 
	//==============================================================

};