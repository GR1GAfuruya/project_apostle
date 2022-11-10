#pragma once
#include "mesh_shader.h"
#include "skeletal_mesh.h"
#include "graphics.h"
#include "imgui_include.h"
#include "user.h"
class MeshEffect
{
private:
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================
	struct CONSTANTS
	{
		DirectX::XMFLOAT4 particle_color = { 0,0,0,1 };
		DirectX::XMFLOAT2 scroll_direction = {0,0};
		float scroll_speed = 0;
		float threshold = 0;
	};
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	//MeshEffect(){}
	MeshEffect(Graphics& graphics, const char* fbx_filename);
	~MeshEffect(){};
	//再生
	void play(DirectX::XMFLOAT3 pos);
	//停止
	void stop();
	//更新
	void update(Graphics& graphics, float elapsed_time);
	//描画
	void render(Graphics& graphics);

	void debug_gui(string str_id);

	//オブジェクトの軸
	enum class AXIS
	{
		RIGHT,
		UP,
		FORWARD,
	};
	//Setter
	void set_position(DirectX::XMFLOAT3 p) { position = p; }
	void set_orientation(DirectX::XMFLOAT4 o) { orientation = o; }
	void set_scale(DirectX::XMFLOAT3 s) { scale = s; }
	void set_scale(float s) { scale = { s,s,s }; }
	void set_velocity(DirectX::XMFLOAT3 v) { velosity = v; }
	void set_life_span(float l) { life_span = l; }
	void set_is_loop(bool loop) { is_loop = loop; }
	void set_rotate_quaternion(DirectX::XMFLOAT3 axis, float ang);
	void set_rotate_quaternion(AXIS axis, float ang);
	void rotate_base_axis(AXIS axis, DirectX::XMFLOAT3 dir_vec);
	void register_shader_resource(ID3D11Device* device, const wchar_t* filename);
	void register_shader_resource(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void create_pixel_shader(ID3D11Device* device, const char* cso_name);
	//Getter
	DirectX::XMFLOAT3 get_position() { return position; }
	DirectX::XMFLOAT4 get_orientation() { return orientation; }
	DirectX::XMFLOAT3 get_scale() { return scale; }
	DirectX::XMFLOAT3 get_velosity() { return velosity; }
	float get_life_time() { return life_time; }
	bool get_active() { return active; }

	void reset_orientation();
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
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> shader_resources;
	std::unique_ptr<MeshShader> shader = nullptr;
	// スケルタルメッシュの実体
	std::unique_ptr <SkeletalMesh> model;
	//位置
	DirectX::XMFLOAT3 position = {0,0,0};
	//回転軸
	DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
	//スケール
	DirectX::XMFLOAT3 scale = {1,1,1};
	//トランスフォーム
	DirectX::XMFLOAT4X4	transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	//速度
	DirectX::XMFLOAT3 velosity = {0,0,0};
	
	//生存時間
	float life_time = 0;
	//寿命
	float life_span = FLT_MAX;
	//アクティブ状態か
	bool active = false;
	//ループ再生するかどうか
	bool is_loop = false;
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
