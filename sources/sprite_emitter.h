#pragma once
#include "particle.h"
#include "material.h"
#include "camera.h"
#include "component.h"
//#include "data_file.h"
class SpriteEmitter :public Component
{
public:
	//==============================================================
	// 
	// 構造体
	// 
	//==============================================================

	struct Param
	{
		//位置
		DirectX::XMFLOAT3 position = { 0,0,0 };
		//射出方向
		DirectX::XMFLOAT3 emit_dir = { 0,0,0 };
		//期間
		float duration = 5;
		float init_speed = 5;
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
	// public関数
	// 
	//==============================================================
	SpriteEmitter(Param init_param, int max_particles);
	~SpriteEmitter()override;

	void play(DirectX::XMFLOAT3 pos);


	void emit(float elapsed_time);
	void start()override;

	void update(float elapsed_time) override;

	void render(Camera* camera);

	void on_gui()override;

	//==============================================================
	// 
	// セッターとゲッター
	// 
	//==============================================================

	//Setter
	inline void set_position(DirectX::XMFLOAT3 p) { position = p; }
	inline void set_emit_span(float span) { param.emit_span = span; }
	inline void set_is_loop(bool loop) { param.is_loop = loop; }
	//Getter
	DirectX::XMFLOAT3 get_position() { return position; }
	float get_duration() { return param.duration; }
	bool get_active() { return active; }

	const char* get_name() const override { return "SpriteEmitter"; }
	Param param;
private:
	// 頂点フォーマット
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		
		DirectX::XMFLOAT2 texcoord;
	};

	struct Instance
	{
		DirectX::XMFLOAT4 quaternion;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT2 tip_texcoord;
		DirectX::XMFLOAT4 color;
	};

	struct OBJECT_CONSTANTS
	{
		DirectX::XMFLOAT4X4 world;
	};

	void position_update(float elapsed_time);
	void life_update(float elapsed_time);
	//姿勢更新
	void orientation_update(float elapsed_time);

	void remove_update();

	void particle_start_functions();

	void particle_update_functions(float elapsed_time, Particles& p);

	void ReplaceBufferContents(ID3D11Buffer* buffer, size_t bufferSize, const void* data);

	void create_com_object(int max_particles);
	//位置
	DirectX::XMFLOAT3 position = { 0,0,0 };
	//姿勢
	DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
	//角度
	DirectX::XMFLOAT3 rotation = { 0,0,0 };

	//大きさ
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	//射出方向
	DirectX::XMFLOAT3 emit_dir = { 0,0,0 };
	//速度
	DirectX::XMFLOAT3 velocity = { 0,0,0 };

	//前回パーティクルを生成してからの時間
	float emit_timer = 0;
	//エミッターが発生してからの時間
	float life_timer = 1;
	//アクティブ状態か
	bool active = false;
	// 現在の発生カウント
	int emit_count =0;
	//時間
	float timer = 0;
	//画像位置
	DirectX::XMFLOAT2 texpos = {  };
	//画像サイズ
	DirectX::XMFLOAT2 texsize = {  };

	float animation_rate = 1;

	//パーティクルを格納するコンテナ
	std::vector<std::unique_ptr<Particles>> particles;
	std::vector<std::unique_ptr<Particles>> removes;
	//COMオブジェクト
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometry_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> instance_data;

	D3D11_TEXTURE2D_DESC texture2d_desc;

	const size_t max_vertices;
	std::vector<vertex> vertices;
	std::unique_ptr<Instance[]> CPU_instance_data;

	const int MATERIAL_START_SLOT = 20;
	//デバッグGUIフラグ
	bool display_imgui = false;
	//子のエミッターが生成できるパーティクルの最大数
	int max_particles;
	//アクティブ状態のパーティクル数
	int active_count;
	//パーティクルの初期パラメーター
	Particles::Param particle_init_param;

};