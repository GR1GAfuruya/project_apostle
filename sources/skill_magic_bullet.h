#pragma once
#include "skill.h"
class MagicBullet : public Skill
{
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	MagicBullet() {}
	~MagicBullet() {}

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
	DirectX::XMFLOAT3 position{};
	DirectX::XMFLOAT3 velocity{};

	//==============================================================
	// 
	// �萔
	// 
	//==============================================================
public:

};