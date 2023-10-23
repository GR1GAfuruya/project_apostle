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

	struct WaveParam
	{
		DirectX::XMFLOAT3 position;
		float height;
		float width;
		float radius;
		float expansion_speed;
		float max_radius;
		bool active = false;
		bool is_hit = false;

	};
	BossAttackSkill1();
	~BossAttackSkill1();

	//発動
	void chant(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir);
	//停止
	void stop();
	//更新
	void update(float elapsed_time, Camera* camera, DirectX::XMFLOAT3 arm_pos, DirectX::XMFLOAT3 arm_dir);
	//描画
	void render(Camera* camera);
	//デバッグGUI
	void debug_gui(const char* str_id);


	//当たり判定
	void calc_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func);


private:
	//弾が当たった時の処理
	void on_hit();

	//チャージ時の更新
	void charge_state_update(float elapsed_time, Camera* camera);
	//攻撃時の更新
	void attack_state_update(float elapsed_time, Camera* camera);

	typedef std::function<void(float elapsed_time, Camera* camera)> StateUpdate;

	StateUpdate state_update;
	float ray_power = 5.0f;
	std::unique_ptr<MeshEffect> arm_effect;
	std::unique_ptr<MeshEffect> wave_effect;
	WaveParam wave_params;

	std::unique_ptr<GPU_Particles> spark_effect;
	//GPUパーティクルのエミッターCS
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	//GPUパーティクルのアップデートCS
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;


	DirectX::XMFLOAT3 arm_pos;
	DirectX::XMFLOAT3 arm_dir;
	DirectX::XMFLOAT3 boss_position;
	float range;
	float charge_timer;
	float charge_time;
	AttackParam at_param;

	//チャージ時のカメラシェイク
	Camera::CameraShakeParam camera_shake = {};
	bool display_imgui;
};