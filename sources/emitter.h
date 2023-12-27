#pragma once
#include "particle.h"
#include "material.h"
#include "camera.h"
#include "component.h"
class Emitter :public Component
{
public:
	Emitter(int max_particles);
	virtual ~Emitter();
	virtual const char* get_name() const override { return "Emitter"; }

	virtual void play(DirectX::XMFLOAT3 pos);

	virtual void start() {}

	virtual void emit(float elapsed_time);

	virtual void update(float elapsed_time);

	virtual void render(Camera& camera);

	virtual void debug_gui(string id);

protected:
	virtual void emitter_update(float elapsed_time) {}

	virtual void particle_spawn(float elapsed_time) {}

	virtual void particle_update(float elapsed_time){}

	//==============================================================
	// 
	// セッターとゲッター
	// 
	//==============================================================

	//Setter
	inline void set_emit_span(float span) { emit_span = span; }
	inline void set_is_loop(bool loop) { is_loop = loop; }

	float get_duration() { return duration; }
	bool get_active() { return active; }

	// 頂点フォーマット

	struct Param
	{
		//位置
		DirectX::XMFLOAT3 position = { 0,0,0 };
		//射出方向
		DirectX::XMFLOAT3 emit_dir = { 0,0,0 };
		//期間
		float duration = 5;
		//ループ再生するかどうか
		bool is_loop = false;
		//生成し続ける時間
		float emit_time;
		// 1秒間に何発発生するか
		float emit_rate;
		// 発生間隔
		float emit_span;
		//一度に放出する数
		int burst_num = 1;
		//生成開始時間
		float emit_start_time = 0;

		int TileX = 1;
		int TileY = 1;
		int tile_num = 0;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				cereal::make_nvp("position", position),
				cereal::make_nvp("emit_dir", emit_dir),
				cereal::make_nvp("duration", duration),
				cereal::make_nvp("emit_span", emit_span),
				cereal::make_nvp("emit_start_time", emit_start_time)
			);
		}

	};
	//==============================================================
	// 
	//　パブリック変数
	// 
	//==============================================================
public:
	Transform transform;
	Param param;
	//==============================================================
	// 
	//　プロテクテッド変数
	// 
	//==============================================================
protected:
	void position_update(float elapsed_time);
	void life_update(float elapsed_time);
	void remove_update();
	//速度
	DirectX::XMFLOAT3 velocity;

	//前回パーティクルを生成してからの時間
	float emit_timer = 0;
	//エミッターが発生してからの時間
	float life_timer = 1;
	//アクティブ状態か
	bool active = false;
	//ループ再生するかどうか
	bool is_loop = false;
	//生成し続ける時間
	float duration;
	// 1秒間に何発発生するか
	float emit_rate; 
	// 発生間隔
	float emit_span;
	// 現在の発生カウント
	int emit_count; 
	//一度に放出する数
	int burst_num = 1;
	//時間
	float timer = 0;
	//生成開始時間
	float emit_start_time;
	
	//パーティクルを格納するコンテナ
	std::vector<std::unique_ptr<Particles>> particles;
	std::vector<std::unique_ptr<Particles>> removes;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	D3D11_TEXTURE2D_DESC texture2d_desc;
	
	
	const int MATERIAL_START_SLOT = 20;
	//デバッグGUIフラグ
	bool display_imgui = false;

	int max_particles;
	int active_count;

};