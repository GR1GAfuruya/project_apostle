#pragma once
#include "graphics.h"
#include "skeletal_mesh.h"
#include "user.h"
class Sword
{
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	Sword() {}
	Sword(Graphics& graphics);
	~Sword() {}

	struct Capsule
	{
		DirectX::XMFLOAT3 start;
		DirectX::XMFLOAT3 end;
		float radius;
	};

	void initialize();
	void update(Graphics& graphics, float elapsed_time);
	void render(Graphics& graphics);

	//装備時の位置取得
	const DirectX::XMFLOAT3& get_equipped_position() const { return DirectX::XMFLOAT3(transform._41, transform._42, transform._43); }
	//剣先へのベクトル取得
	const DirectX::XMFLOAT3& get_dir_sword_top() const { return Math::get_posture_forward(transform); }
	const Capsule& get_collision() const { return collision; };
	//位置設定
	void set_position(const DirectX::XMFLOAT3& position) { this->position = position; }
	//剣の向きをボーンのクォータニオンに合わせる
	void set_parent_transform(const DirectX::XMFLOAT4X4& parent_transform) { this->parent_transform = parent_transform; }
private:

	//==============================================================
	// 
	// 変数
	// 
	//==============================================================

	DirectX::XMFLOAT3	position = { 0, 0, 0 };
	DirectX::XMFLOAT3	scale = { 1, 1, 1 };
	DirectX::XMFLOAT4 orientation{ 0,0,0,1 };
	DirectX::XMFLOAT4X4	transform = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	};

	DirectX::XMFLOAT4X4	parent_transform = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	};

	// スケルタルメッシュの実体
	std::unique_ptr <SkeletalMesh> model;
	Capsule collision;

	//剣の長さ
	float length;
	//剣の太さ
	float radius;
};
