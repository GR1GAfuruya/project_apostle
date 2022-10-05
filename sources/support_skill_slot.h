#pragma once
#include "skill_slot.h"
//#define SP_SKILLTYPE SupportSkillSlot::SupportSkillType

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
	void chant(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3* target_pos);
	//==============================================================
	// 
	// private�֐�
	// 
	//==============================================================
	//�X�L���̎��̐���
	void entity_generation_by_type(Graphics& graphics, std::unique_ptr<Skill>& coffin, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3* target_pos) ;
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