#pragma once
#include "mesh_effect.h"
#include "gpu_particle.h"
#include "meteore.h"
#include "light.h"
#include "camera.h"
class BossAttackSkill3
{
	//==============================================================
	// 
	// public�֐�
	// 
	//=============================================================
public:
	BossAttackSkill3(Graphics& graphics);
	~BossAttackSkill3() {}

	//����
	void chant(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir);
	//��~
	void stop();
	//�X�V
	void update(Graphics& graphics, float elapsed_time, Camera* camera);
	//�`��
	void render(Graphics& graphics);
	//�f�o�b�OGUI
	void debug_gui(const char* str_id);

};