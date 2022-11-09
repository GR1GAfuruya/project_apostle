#pragma once
#include "mesh_effect.h"
#include "move_behavior.h"
class Meteore :public MoveBehavior
{
public:
	Meteore(Graphics& graphics);
	~Meteore() {};

	void initialize();
	void update(Graphics& graphics, float elapsed_time);
	void render(Graphics& graphics);

	void launch(DirectX::XMFLOAT3 init_pos, DirectX::XMFLOAT3 init_vec, float speed);

	//位置取得
	const DirectX::XMFLOAT3& get_position() const { return position; }
	//位置設定
	void set_position(const DirectX::XMFLOAT3& position) { this->position = position; }
	// スケール取得
	const DirectX::XMFLOAT3& get_scale() const { return scale; }
	//velocity取得
	const DirectX::XMFLOAT3& get_velocity() const { return velocity; }
	//スケール設定
	void set_scale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	void update_velocity(float elapsed_time, DirectX::XMFLOAT3& position);

	bool get_is_hit() const { return is_hit; }
private:

	void move(float vx, float vz, float speed);
	//垂直速力更新処理
	void update_vertical_velocity(float elapsed_frame) override;
	//垂直移動更新処理
	void update_vertical_move(float elapsed_time, DirectX::XMFLOAT3& position)override;
	//水平速力更新処理
	void update_hrizontal_velocity(float elapsed_frame) override;
	//水平移動更新処理
	void update_horizontal_move(float elapsed_time, DirectX::XMFLOAT3& position) override;

	std::unique_ptr<MeshEffect> main_effect;

	float ray_power = 5.0f;

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	bool is_hit = false;
};