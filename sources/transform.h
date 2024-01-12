#pragma once

class Transform
{
public:
	Transform() : position(0, 0, 0)
		, orientation(0, 0, 0, 1)
		, scale(1, 1, 1)
		, transform(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		) {};

	 ~Transform(){};


	void on_gui()
	{
		ImGui::Begin("transform");
		ImGui::DragFloat3("position", &position.x);
		ImGui::DragFloat4("orientation", &orientation.x);
		ImGui::DragFloat3("scale", &scale.x);
		ImGui::End();
	}

	void set_position(DirectX::XMFLOAT3 position) { this->position = position; }
	void set_orientation(DirectX::XMFLOAT4 orientation) { this->orientation = orientation; }
	void set_scale(DirectX::XMFLOAT3 scale) { this->scale = scale; }
	void set_transform(DirectX::XMFLOAT4X4 tra) { this->transform = tra; }
	void set_parent_transform(std::shared_ptr<DirectX::XMFLOAT4X4> parent) { this->is_parent = true; this->parent_transform = parent; }
	void set_is_parent(bool p) { this->is_parent = p; }
	const DirectX::XMFLOAT3 get_position() const { return position; }
	const DirectX::XMFLOAT4 get_orientation() const { return orientation; }
	const DirectX::XMFLOAT3 get_scale() const { return scale; }
	const DirectX::XMFLOAT4X4 get_transform() const { return transform; }
	const std::shared_ptr<DirectX::XMFLOAT4X4> get_parent_transform() const { return parent_transform; }
	const bool get_is_parent() const { return is_parent; }


private:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
	};
	std::shared_ptr<DirectX::XMFLOAT4X4> parent_transform;
	bool is_parent =false;
};