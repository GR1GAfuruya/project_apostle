#pragma once

#include <DirectXMath.h>
#include "move_behavior.h"
#include "generic_frequent_entities.h"

#include "damage_func.h"

//キャラクター
class Charactor :public GenericFrequentEntities
{
	
public:
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================
	struct CharactorParam
	{
		//キャラクターの半径
		float radius = 1.0f;
		//キャラの縦の大きさ
		float height = 2.6f;
		//最大体力
		int max_health = 1000;
		//摩擦力
		float friction = 1.0f;
		//空気抵抗
		float air_control = 0.3f;
		//加速度
		float acceleration = 1.5f;
		//最大速度
		float max_move_speed = 30.0f;
		//回転速度
		float turn_speed;
		//移動速度
		float move_speed;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				cereal::make_nvp("radius", radius),
				cereal::make_nvp("height", height),
				cereal::make_nvp("max_health", max_health),
				cereal::make_nvp("friction", friction),
				cereal::make_nvp("air_control", air_control),
				cereal::make_nvp("acceleration", acceleration),
				cereal::make_nvp("max_move_speed", max_move_speed),
				cereal::make_nvp("turn_speed", turn_speed),
				cereal::make_nvp("move_speed", move_speed)
			);
		}

	};

	//==============================================================
	// 
	// コンストラク, デストラクタ
	// 
	//==============================================================
	Charactor() {}
	virtual ~Charactor() {}

	//==============================================================
	// 
	// セッターとゲッター
	// 
	//==============================================================

	//位置取得
	const DirectX::XMFLOAT3& get_position() const { return position; }
	//位置設定
	void set_position(const DirectX::XMFLOAT3& position) { this->position = position; }
	// 回転取得
	const DirectX::XMFLOAT3& get_angle() const { return angle; }
	//回転設定
	void set_angle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
	// スケール取得
	const DirectX::XMFLOAT3& get_scale() const { return scale; }
	//velocity取得
	const DirectX::XMFLOAT3& get_velocity() const { return velocity; }
	//スケール設定
	void set_scale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	//velocityセット
	void set_velocity(const DirectX::XMFLOAT3& v) { this->velocity = v; }
	// 半径
	float get_radius() const { return chara_param.radius; }
	// HP
	int get_health() const { return health; }
	// 最大HP
	int get_max_health() const { return chara_param.max_health; }
	//体の正面と進行方面との角度
	float get_turn_angle() const { return turn_angle; }
	//HPパーセンテージ
	float get_hp_percent() const { return health <= 0 ? 0.0f : static_cast<float>(health) / static_cast<float>(chara_param.max_health); }
	// 地面判定
	bool get_is_ground() const { return is_ground; }
	//高さ取得
	float get_height() const { return chara_param.height; }
	//トランスフォームのゲッター
	const DirectX::XMFLOAT4X4& get_transform() const { return transform; }
	//衝撃を与える
	void add_impulse(const DirectX::XMFLOAT3& impulse);
	//ダメージを与える
	bool apply_damage(int damage, float invincibleTime, WINCE_TYPE type);

protected:

	void Move(float vx, float vz, float speed);
	void Turn(float elapsed_time, float vx, float vz, float speed);//オイラー
	void Turn(float elapsed_time, DirectX::XMFLOAT3 move_vec, float speed, DirectX::XMFLOAT4& orien);//クォータニオン
	//ジャンプ処理
	void Jump(float speed);
	//速力処理更新
	void update_velocity(float elapsed_time, DirectX::XMFLOAT3& position);
	virtual void on_landing() {}
	//死亡したときに呼ばれる
	virtual void on_dead() {}
	virtual void on_damaged(WINCE_TYPE type) {}
	void update_invicible_timer(float elapsed_time);



	//-----------変数--------------//

	DirectX::XMFLOAT3	position = { 0, 0, 0 };
	DirectX::XMFLOAT3	angle = { 0, 0, 0 };
	DirectX::XMFLOAT3	scale = { 1, 1, 1 };
	DirectX::XMFLOAT4 orientation{ 0,0,0,1 };
	DirectX::XMFLOAT4X4	transform = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	};

	CharactorParam chara_param;

	float stepOffset = 0.7f;
	DirectX::XMFLOAT3 velocity = { 0, 0, 0 };
	//地面に当たっているか
	bool is_ground = false;

	float invincible_timer = 0.0f;
	float move_vec_x = 0.0f;
	float move_vec_z = 0.0f;

	//体力
	int32_t health;


	float vs_wall_ray_power = 5.0f;
	//坂の法線
	DirectX::XMFLOAT3 slope_normal = {};
	float gravity = -1.0f;
	float turn_angle;			//体を向けるときの回転角
	//-----------プライベート関数--------------//
private:
	//垂直速力更新処理
	void update_vertical_velocity(float elapsed_flame) ;
	//垂直移動更新処理
	void update_vertical_move(float elapsed_time, DirectX::XMFLOAT3& position);
	//水平速力更新処理
	void update_hrizontal_velocity(float elapsed_frame) ;
	//水平移動更新処理
	void update_horizontal_move(float elapsed_time, DirectX::XMFLOAT3& position) ;
};
