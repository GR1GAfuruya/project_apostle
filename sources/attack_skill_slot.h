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
	void chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3* target_pos);
	//�X�L���̎��̐���
	void entity_generation_by_type(Graphics& graphics, std::unique_ptr<Skill>& coffin, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3* target_pos);
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
