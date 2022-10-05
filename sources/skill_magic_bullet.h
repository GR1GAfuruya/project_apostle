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
	MagicBullet(){}
	MagicBullet(Graphics& graphics, DirectX::XMFLOAT3 init_pos, DirectX::XMFLOAT3 dir);
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
	float acceleration;

	//==============================================================
	// 
	// �萔
	// 
	//==============================================================
public:

};