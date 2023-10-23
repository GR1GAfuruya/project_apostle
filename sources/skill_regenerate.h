#pragma once
#include "skill.h"

class Regenerate : public Skill
{
public:
	//==============================================================
	// 
	// public�֐�
	// 
	//==============================================================
	Regenerate(DirectX::XMFLOAT3* launch_pos, int* health, const int max_health);
	~Regenerate();

	void initialize() override;
	void update(float elapsed_time)override;
	void render(Camera* camera)override;

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
	//�񕜂�����Ώۂ̗̑͂̃|�C���^
	std::unique_ptr<int> charactor_health;
	//���C�g
	std::shared_ptr<PointLight> light;
	//���C�g�ʒu
	std::unique_ptr<DirectX::XMFLOAT3> light_position;
	//�̗͂̍ő�l
	int recovery_max_health;
	//�񕜎���
	float recovery_timer;
	//�񕜗�
	int recovery_rate;
	//==============================================================
	// 
	// �萔
	// 
	//==============================================================
private:
	static constexpr float RECOVERY_SPAN = 1.0f;
};