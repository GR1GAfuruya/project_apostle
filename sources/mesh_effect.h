#pragma once
#include "mesh_shader.h"
#include "skeletal_mesh.h"
#include "graphics.h"
#include "imgui_include.h"
#include "user.h"
#include "material_manager.h"
#include "camera.h"
class MeshEffect
{
	public:
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================
	struct CONSTANTS
	{
		DirectX::XMFLOAT4 particle_color = { 0,0,0,1 };
		DirectX::XMFLOAT2 scroll_direction = { 0,0 };
		float scroll_speed = 0;
		float threshold = 0;
	};

	struct EffectParam
	{
		//位置
		DirectX::XMFLOAT3 position = { 0,0,0 };
		//回転軸
		DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
		//スケール
		DirectX::XMFLOAT3 scale = { 1,1,1 };
		//速度
		DirectX::XMFLOAT3 velosity = { 0,0,0 };
		//カラー
		DirectX::XMFLOAT4 color = { 0,0,0,1 };
		//生存期間
		float life_duration = FLT_MAX;
	};


	//オブジェクトの軸
	enum class AXIS
	{
		RIGHT,
		UP,
		FORWARD,
	};


public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	MeshEffect(const char* fbx_filename);
	~MeshEffect(){};
	//再生
	void play(DirectX::XMFLOAT3 pos);
	//停止
	void stop();
	//更新
	void update(float elapsed_time);
	//描画
	void render();//フラスタムカリング無
	void render(Camera* camera);//フラスタムカリングあり

	void debug_gui(string str_id);


	//初期化関数
	void set_init_position(DirectX::XMFLOAT3 p) { init_effect_param.position = p; }
	void set_init_orientation(DirectX::XMFLOAT4 o) { init_effect_param.orientation = o; }
	void set_init_scale(DirectX::XMFLOAT3 s) { init_effect_param.scale = s; }
	void set_init_scale(float s) { init_effect_param.scale = { s,s,s }; }
	void set_init_color(DirectX::XMFLOAT4 c) { init_effect_param.color = c; }
	void set_init_velocity(DirectX::XMFLOAT3 v) { init_effect_param.velosity = v; }
	void set_init_life_duration(float d) { init_effect_param.life_duration = d; }
	void set_life_duration(float d) { effect_param.life_duration = d; }
	//Setter
	void set_position(DirectX::XMFLOAT3 p) { effect_param.position = p; }
	void set_orientation(DirectX::XMFLOAT4 o) { effect_param.orientation = o; }
	void set_scale(DirectX::XMFLOAT3 s) { effect_param.scale = s; }
	void set_scale(float s) { effect_param.scale = { s,s,s }; }
	void set_color(DirectX::XMFLOAT4 c) { constants.get()->data.particle_color = c; }
	void set_velocity(DirectX::XMFLOAT3 v) { effect_param.velosity = v; }
	void set_is_loop(bool loop) { is_loop = loop; }
	void set_rotate_quaternion(DirectX::XMFLOAT3 axis, float ang);
	void set_rotate_quaternion(AXIS axis, float ang);
	void rotate_base_axis(AXIS axis, DirectX::XMFLOAT3 dir_vec);
	void set_material(Material* m) { material = m; };
	void set_dissolve_flag(bool flag) { is_dissolve = flag; };
	//Getter
	DirectX::XMFLOAT3 get_position() { return effect_param.position; }
	DirectX::XMFLOAT4 get_orientation() { return effect_param.orientation; }
	DirectX::XMFLOAT3 get_scale() { return effect_param.scale; }
	DirectX::XMFLOAT3 get_velosity() { return effect_param.velosity; }
	DirectX::XMFLOAT4 get_color() { return constants.get()->data.particle_color; }
	float get_life_rate() { return life_time/ effect_param.life_duration; }
	float get_life_time() { return life_time; }
	bool get_active() { return active; }
	Material get_material() { return *material; }
	void reset_orientation();
protected:
	void dissolve_update(float elapsed_time);

	//==============================================================
	// 
	// 変数
	// 
	//==============================================================
public:
	std::unique_ptr<Constants<CONSTANTS>> constants{};
	//回転スピード
	DirectX::XMFLOAT3 rot_speed = { 0,0,0 };
protected:

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	//Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	//std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> shader_resources;
	std::unique_ptr<MeshShader> shader = nullptr;
	// スケルタルメッシュの実体
	std::unique_ptr <SkeletalMesh> model;

	Material* material;
	EffectParam init_effect_param;
	EffectParam effect_param;
	
	//トランスフォーム
	DirectX::XMFLOAT4X4	transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	
	
	//生存時間
	float life_time = 0;
	//アクティブ状態か
	bool active = false;
	//ループ再生するかどうか
	bool is_loop = false;
	//ディゾルブするかどうか
	bool is_dissolve = true;
	//デバッグGUIフラグ
	bool display_imgui = false;

	string instance_id;
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================

	const int MATERIAL_START_SLOT = 20;

};
