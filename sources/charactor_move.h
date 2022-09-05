#pragma once

#include <DirectXMath.h>
#include "move_behavior.h"
//キャラクター
class CharactorMove :public MoveBehavior
{
public:
	CharactorMove() {}
	virtual ~CharactorMove(){}
	//行列更新処理
	void update_transform();
	//行列更新処理
	//第二引数が原点にずらす値
	//第三引数がずらした後の移動値
	void update_transform(const DirectX::XMFLOAT4& orien, const DirectX::XMFLOAT3& pos = {1,1,1});
	//垂直速力更新処理
	void update_vertical_velocity(float elapsed_flame) override;
	//垂直移動更新処理
	void update_vertical_move(float elapsed_time, DirectX::XMFLOAT3& position, Stage* stage)override;
	//水平速力更新処理
	void update_hrizontal_velocity(float elapsed_time) override;
	//水平移動更新処理
	void update_horizontal_move(float elapsed_time, DirectX::XMFLOAT3& position, Stage* stage) override;

	//行列更新処理
//	void update_transform(const DirectX::XMFLOAT4& orien, const DirectX::XMFLOAT3& pos = {1,1,1}, const DirectX::XMFLOAT4X4& transform = {});
	//位置取得
	const DirectX::XMFLOAT3& get_position() const { return position; }
	//位置設定
	void set_position(const DirectX::XMFLOAT3& position) { this->position = position; }
	// 回転取得
	const DirectX::XMFLOAT3 & get_angle() const { return angle; }
	//回転設定
	void set_angle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
	// スケール取得
	const DirectX::XMFLOAT3 & get_scale() const { return scale; }
	//velocity取得
	const DirectX::XMFLOAT3& get_velocity() const { return velocity; }
	//スケール設定
	void set_scale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	//velocityセット
	void set_velocity(const DirectX::XMFLOAT3& v) { this->velocity = v; }
	// 半径
	float get_radius() const {return radius;}
	// HP
	int get_health() const { return health; }
	// 最大HP
	uint32_t GetMaxHealth() const { return max_health; }
	// 地面判定
	bool get_is_ground() const { return is_ground; }
	//高さ取得
	float get_height() const { return height; }
	//トランスフォームのゲッター
	const DirectX::XMFLOAT4X4& get_transform() const { return transform; }
	//衝撃を与える
	void add_impulse(const DirectX::XMFLOAT3& impulse);
	//ダメージを与える
	bool apply_damage(int damage, float invincibleTime);
protected:
	float stepOffset = 0.7f;
	//キャラの頭のてっぺん
	float head_top = 1.0f;
private:
	

protected:
	//void Move(float elpsedTime, float vx, float vz, float speed);
	void Move(float vx, float vz, float speed);
	void Turn(float elapsed_time, float vx, float vz, float speed);//オイラー
	void Turn(float elapsed_time, DirectX::XMFLOAT3 move_vec, float speed, DirectX::XMFLOAT4& orien);//クォータニオン
	//ジャンプ処理
	void Jump(float speed);
	//速力処理更新
	void update_velocity(float elapsed_time, DirectX::XMFLOAT3& position, Stage* stage);
	virtual void on_landing() {}
	//ダメージを受けたときに呼ばれる
	virtual void on_damaged(){}
	//死亡したときに呼ばれる
	virtual void OnDead() {}
	void update_invicible_timer(float elapsed_time);

protected:
	DirectX::XMFLOAT3	position = { 0, 0, 0 };
	DirectX::XMFLOAT3	angle = { 0, 0, 0 };
	DirectX::XMFLOAT3	scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4	transform = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	};
	DirectX::XMFLOAT4 orientation{0,0,0,1};

	float radius = 0.5f;
	//重力
	float gravity = -1.0;
	DirectX::XMFLOAT3 velocity = { 0, 0, 0 };
	//地面に当たっているか
	bool is_ground = false;
	//キャラの縦の大きさ
	float height = 2.2f;
	//体力
	int32_t health = 1000;
	int32_t max_health = 1000;

	float invinsible_timer = 0.0f;
	//摩擦力
	float friction = 1.0f;
	//加速度
	float acceleration = 1.5f;
	float max_move_speed = 30.0f;
	float move_vec_x = 0.0f;
	float move_vec_z = 0.0f;

	float air_control = 0.3f;

	//法線
	DirectX::XMFLOAT3 slope_normal = {};

	

};
