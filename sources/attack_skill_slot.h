#pragma once
#include "skill_slot.h"
#define AT_SKILLTYPE AttackSkillSlot::AttackSkillType

class AttackSkillSlot : public SkillSlot
{
public:
	//==============================================================
	// 
	// �\���́A�񋓌^
	// 
	//==============================================================

	enum class AttackSkillType
	{
		MAGICBULLET,
		SPEARS_SEA
	};
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	AttackSkillSlot(Graphics& graphics, AttackSkillType type);
	~AttackSkillSlot() {}
	//�X�L���̔���
	void chant(Graphics& graphics);
	//�X�L���̎��̐���
	void entity_generation_by_type(Graphics& graphics, std::unique_ptr<Skill>& coffin);
	//==============================================================
	// 
	// �ϐ�
	// 
	//==============================================================

	AttackSkillType skill_type;
	//==============================================================
	// 
	// �萔
	// 
	//==============================================================
};
