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
	// public関数
	// 
	//=============================================================
public:
	BossAttackSkill3(Graphics& graphics);
	~BossAttackSkill3() {}

	//発動
	void chant(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir);
	//停止
	void stop();
	//更新
	void update(Graphics& graphics, float elapsed_time, Camera* camera);
	//描画
	void render(Graphics& graphics);
	//デバッグGUI
	void debug_gui(const char* str_id);

};