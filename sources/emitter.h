#pragma once
#include "particle.h"
#include "material.h"
#include "camera.h"
class Emitter
{
public:
	Emitter(Graphics& graphics, int max_particles);
	~Emitter();

	void create_emitter(Graphics& graphics);

	void emit(Graphics& graphics);

	void update(Graphics& graphics, float elapsed_time);

	void render(Graphics& graphics, Camera& camera);

	void debug_gui(Graphics& graphics, string id);

	//==============================================================
	// 
	// セッターとゲッター
	// 
	//==============================================================

	//Setter
	inline void set_position(DirectX::XMFLOAT3 p) { position = p; }
	inline void set_emit_span(float span) { emit_span = span; }
	inline void set_is_loop(bool loop) { is_loop = loop; }
	inline void set_material(Material* m) { material = m; };
	//Getter
	DirectX::XMFLOAT3 get_position() { return position; }
	float get_life_time() { return life_time; }
	bool get_active() { return active; }

	struct InitParam
	{
		//位置
		DirectX::XMFLOAT3 position = { 0,0,0 };
		//射出方向
		DirectX::XMFLOAT3 emit_dir = { 0,0,0 };
		//寿命
		float life_time = FLT_MAX;
		//ループ再生するかどうか
		bool is_loop = false;
		//生成し続ける時間
		float emit_time;
		// 1秒間に何発発生するか
		float emit_rate;
		// 発生間隔
		float emit_span;

	};

private:
	void position_update(float elapsed_time);
	void life_update(float elapsed_time);
	void remove_update();

	//位置
	DirectX::XMFLOAT3 position = { 0,0,0 };
	//射出方向
	DirectX::XMFLOAT3 emit_dir = { 0,0,0 };
	//速度
	DirectX::XMFLOAT3 velocity;

	//生成されてからの時間
	float timer = 0;
	//寿命
	float life_time = 1;
	//アクティブ状態か
	bool active = false;
	//ループ再生するかどうか
	bool is_loop = false;
	//生成し続ける時間
	float emit_time;
	// 1秒間に何発発生するか
	float emit_rate; 
	// 発生間隔
	float emit_span;
	// 現在の発生カウント
	int emit_count; 

	//パーティクルを格納するコンテナ
	std::vector<std::unique_ptr<Particles>> particles;
	std::vector<std::unique_ptr<Particles>> removes;

	Material* material;
	std::unique_ptr<SpriteBatch> sprite = nullptr;

	const int MATERIAL_START_SLOT = 20;
	//デバッグGUIフラグ
	bool display_imgui = false;

	int max_particles;

};