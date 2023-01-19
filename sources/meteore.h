#pragma once
#include "instance_mesh_effect.h"
#include "move_behavior.h"
#include "primitive.h"
#include "damage_func.h"
#include"mesh_effect.h"
class Meteore :public MoveBehavior
{
public:
	//==============================================================
	// 
	// 構造体
	// 
	//==============================================================
	struct MeteoreParam
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 velocity;
		DirectX::XMFLOAT3 move_vec;
		DirectX::XMFLOAT3 scale;
		bool is_calc_velocity = false;
		bool is_hit = false;
		Sphere colider_sphere;
	};

	//==============================================================
	// 
	// public関数
	// 
	//==============================================================

	Meteore(Graphics& graphics,int max_num);
	~Meteore() {};

	void initialize();
	void update(Graphics& graphics, float elapsed_time);
	void render(Graphics& graphics);
	void debug_gui();
	//生成
	void create(DirectX::XMFLOAT3 position, int index) { params[index].position = position; }
	void create_on_circle(DirectX::XMFLOAT3 center, float radius, int index);

	//上昇
	void rising(float elapsed_time,DirectX::XMFLOAT3 target_position, float target_scale, float rise_speed,int index);

	//個別に射出
	void launch(DirectX::XMFLOAT3 init_vec, float speed,int index);
	//一斉射出
	void all_launch(DirectX::XMFLOAT3 init_vec, float speed);

	//位置取得
	const DirectX::XMFLOAT3& get_position(int index) const { return params[index].position; }
	//位置設定
	void set_position(const DirectX::XMFLOAT3& position, int index) { this->params[index].position = position; }
	// スケール取得
	const DirectX::XMFLOAT3& get_scale(int index) const { return params[index].scale; }
	//velocity取得
	const DirectX::XMFLOAT3& get_velocity(int index) const { return params[index].velocity; }

	const int get_max_num() const { return MAX_NUM; }
	//スケール設定
	void set_scale(const DirectX::XMFLOAT3& scale, int index) { this->params[index].scale = scale; }
	//発射中のみ速度計算
	void update_velocity(float elapsed_time, int index);
	//プレイヤーの攻撃との当たり判定
	void calc_meteore_vs_player(DirectX::XMFLOAT3 capsule_start, DirectX::XMFLOAT3 capsule_end, float colider_radius, AddDamageFunc damaged_func);


private:
	//==============================================================
	// 
	// private関数
	// 
	//==============================================================
	//移動に関するパラメーターをセット
	void move(float vx, float vz, float speed, int index);
	//垂直速力更新処理
	void update_vertical_velocity(float elapsed_frame, int index) ;
	//垂直移動更新処理
	void update_vertical_move(float elapsed_time, int index);
	//水平速力更新処理
	void update_hrizontal_velocity(float elapsed_frame, int index) ;
	//水平移動更新処理
	void update_horizontal_move(float elapsed_time, int index) ;
	//隕石が当たった時の処理
	void on_hit(int index);
	//==============================================================
	// 
	// private変数
	// 
	//==============================================================
	std::unique_ptr<InstanceMeshEffect> main_effect;
	std::vector<std::unique_ptr<MeshEffect>> meteo_wave;
	std::unique_ptr<MeteoreParam[]> params;
	float ray_power = 5.0f;

	int MAX_NUM;
	float radius;
	//攻撃力
	int power;
	//付与無敵時間
	float invinsible_time;

	bool show_imgui;

};