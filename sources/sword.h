#pragma once
#include "graphics.h"
#include "skeletal_mesh.h"
class Sword
{
public:
	Sword() {}
	Sword(Graphics& graphics);
	~Sword() {}

	void initialize();
	void update(Graphics& graphics, float elapsed_time);
	void render(Graphics& graphics);

	//位置取得
	const DirectX::XMFLOAT3& get_position() const { return position; }
	//姿勢取得
	const DirectX::XMFLOAT4& get_orientation() const { return orientation; }
	//位置設定
	void set_position(const DirectX::XMFLOAT3& position) { this->position = position; }
	//剣の向きをボーンのクォータニオンに合わせる
	void set_parent_transform(const DirectX::XMFLOAT4X4& parent_transform) { this->parent_transform = parent_transform; }
	void set_sword_dir(const DirectX::XMFLOAT3 dir);
private:

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
};
