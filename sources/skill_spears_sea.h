#pragma once
#include "skill.h"
class SpearsSea : public Skill
{
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	SpearsSea() {}
	SpearsSea(Graphics& graphics, DirectX::XMFLOAT3 launch_pos);
	~SpearsSea() {}

	void initialize(Graphics& graphics) override;
	void update(Graphics& graphics, float elapsed_time)override;
	void render(Graphics& graphics)override;

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
	DirectX::XMFLOAT3 pos;
	//==============================================================
	// 
	// �萔
	// 
	//==============================================================
public:

};