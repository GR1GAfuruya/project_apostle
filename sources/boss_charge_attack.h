#pragma once
#include "mesh_effect.h"
#include "gpu_particle.h"
#include "meteore.h"
#include "light.h"
#include "camera.h"
class ChargeAttack
{
public:
	//コンストラクタ
	ChargeAttack(Graphics& grapghics);
	//デストラクタ
	~ChargeAttack(){}
	//発動
	void chant(DirectX::XMFLOAT3 pos);
	//停止
	void stop();
	//更新
	void update(Graphics& graphics, float elapsed_time,Camera* camera);
	//描画
	void render(Graphics& graphics);
	//デバッグGUI
	void debug_gui(const char* str_id);
	//チャージ完了したかどうか
	void set_charge_max_state() { is_charge_max = true; }
	//攻撃対象設定
	void set_target_pos(DirectX::XMFLOAT3 target) { target_pos = target; }
	//当たり判定
	void calc_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func);

private:
	//チャージ中の更新
	void charging_update(Graphics& graphics, float elapsed_time, Camera* camera);
	//発動中の更新
	void activities_update(Graphics& graphics, float elapsed_time, Camera* camera);
	//消滅時の更新
	void vanishing_update(Graphics& graphics, float elapsed_time, Camera* camera);

	typedef void (ChargeAttack::* ChargeAttackUpdate)(Graphics& graphics, float elapsed_time, Camera* camera);
	ChargeAttackUpdate charge_attack_update = &ChargeAttack::charging_update;
	struct ChargeAttackConstants
	{
		DirectX::XMFLOAT3 core_pos{};
		float core_gravitation = 0.5;
		DirectX::XMFLOAT3 particle_sub_color{};
		float core_radius{};
	};
	//コア
	std::unique_ptr<MeshEffect> core;
	//波動
	std::unique_ptr<MeshEffect> wave;
	//トルネード
	std::unique_ptr<MeshEffect> tornado;
	std::unique_ptr<MeshEffect> tornado_black;

	//予兆エフェクト
	std::unique_ptr<MeshEffect> omen_effect;
	//GPUパーティクルのエミッターCS
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	//GPUパーティクルのアップデートCS
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;
	//GPUパーティクル
	std::unique_ptr<GPU_Particles> particle;
	//メテオ
	unique_ptr<Meteore> meteores;
	float meteo_time = 0;
	float meteo_span = 0;
	float meteo_launch_radius = 0;
	int meteo_launch_count = 0;
	//---<PointLight>-----//
	//
	shared_ptr<PointLight> boss_light;

	//定数バッファ
	std::unique_ptr<Constants<ChargeAttackConstants>> constants;
	const float ATTACK_TIME = 3.0f;
	bool is_charge_max = false;
	float attack_time = 0.0f;
	//位置
	DirectX::XMFLOAT3 position = { 0,0,0 };
	//回転軸
	DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
	//スケール
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	static constexpr DirectX::XMFLOAT4 FIRE_COLOR = { 4.0f, 1.0f, 0.7f, 0.8f };
	//デバッグGUIフラグ
	bool display_imgui = false;

	//生存時間
	float life_time = 0;
	//アクティブ状態か
	bool active = false;

	//攻撃対象の座標
	DirectX::XMFLOAT3 target_pos;
	AttackParam attack_param;
	Sphere tornado_colider;
};