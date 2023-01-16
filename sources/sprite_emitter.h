#pragma once
#include "particle.h"
#include "material.h"
#include "camera.h"
class SpriteEmitter
{
public:
	SpriteEmitter(Graphics& graphics, int max_particles);
	~SpriteEmitter();

	void play(DirectX::XMFLOAT3 pos);


	void emit(Graphics& graphics, float elapsed_time);

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
	//Getter
	DirectX::XMFLOAT3 get_position() { return position; }
	float get_duration() { return duration; }
	bool get_active() { return active; }

	// 頂点フォーマット
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	struct InitParam
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
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	//ID3D11Buffer* vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	D3D11_TEXTURE2D_DESC texture2d_desc;

	const size_t max_vertices;
	vertex* vertices;

	const int MATERIAL_START_SLOT = 20;
	//デバッグGUIフラグ
	bool display_imgui = false;

	int max_particles;
	int active_count;

};