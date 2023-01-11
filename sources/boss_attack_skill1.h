#pragma once
#include "mesh_effect.h"
#include "gpu_particle.h"
#include "meteore.h"
#include "light.h"
#include "camera.h"
#include <functional>
class BossAttackSkill1
{
	//==============================================================
	// 
	// public関数
	// 
	//=============================================================
public:
	BossAttackSkill1(Graphics& graphics);
	~BossAttackSkill1() {}

	//発動
	void chant(DirectX::XMFLOAT3 pos);
	//停止
	void stop();
	//更新
	void update(Graphics& graphics, float elapsed_time, Camera* camera);
	//描画
	void render(Graphics& graphics);
	//デバッグGUI
	void debug_gui(const char* str_id);

	void set_hand_pos(DirectX::XMFLOAT3 pos) { hand_pos = pos; }


	//当たり判定
	void calc_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func);

private:
	//チャージ時の更新
	void charge_state_update(Graphics& graphics, float elapsed_time, Camera* camera);
	//攻撃時の更新
	void attack_state_update(Graphics& graphics, float elapsed_time, Camera* camera);

	typedef std::function<void(Graphics& graphics, float elapsed_time, Camera* camera)> StateUpdate;

	StateUpdate state_update;

	std::unique_ptr<GPU_Particles> spark_effect;
	//GPUパーティクルのエミッターCS
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	//GPUパーティクルのアップデートCS
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;

	float range;

	DirectX::XMFLOAT3 hand_pos;

};