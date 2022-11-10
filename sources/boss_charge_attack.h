#pragma once
#include "mesh_effect.h"
#include "gpu_particle.h"
#include "meteore.h"
class ChargeAttack
{
public:
	ChargeAttack(Graphics& grapghics);
	~ChargeAttack(){}

	void play(DirectX::XMFLOAT3 pos);
	void stop();
	void update(Graphics& graphics, float elapsed_time);
	void render(Graphics& graphics);
	void debug_gui(const char* str_id);
	
	void set_charge_max_state() { is_charge_max = true; }
	void set_target_pos(DirectX::XMFLOAT3 target) { target_pos = target; }
private:
	void charging_update(Graphics& graphics, float elapsed_time);
	void activities_update(Graphics& graphics, float elapsed_time);
	void vanishing_update(Graphics& graphics, float elapsed_time);

	typedef void (ChargeAttack::* ChargeAttackUpdate)(Graphics& graphics, float elapsed_time);
	ChargeAttackUpdate charge_attack_update = &ChargeAttack::charging_update;
	struct ChargeAttackConstants
	{
		DirectX::XMFLOAT3 core_pos{};
		float core_gravitation = 0.5;
		float core_radius{};
		DirectX::XMFLOAT3 pad{};
	};
	//地面からコアに伸びる支柱
	std::unique_ptr<MeshEffect> aura[2];
	//コア
	std::unique_ptr<MeshEffect> core;

	std::unique_ptr<MeshEffect> wave;

	std::unique_ptr<MeshEffect> tornado;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> emit_cs;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> update_cs;

	std::unique_ptr<GPU_Particles> particle;
	std::vector<unique_ptr<Meteore>> meteores;
	float meteo_time = 0;
	float meteo_span = 0;
	std::unique_ptr<Constants<ChargeAttackConstants>> constants;
	const float ATTACK_TIME = 2.0f;
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
};