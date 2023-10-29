#pragma once
#include "component.h"

class Transform :public Component
{
public:
	Transform() : position(0, 0, 0 )
				, orientation(0,0,0,1)
				, scale(1,1,1)
				, transform(){};

	virtual ~Transform(){};

	const char* get_name() const override { return "Transform"; }

	void start()override {};
	void update(float elapsed_time)override {};
	void on_gui()override
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
	void set_transform(DirectX::XMFLOAT4X4 scale) { this->transform = transform; }
	void set_parent_transform(std::shared_ptr<Transform> parent) { this->parent_transform = parent.get(); }
	const DirectX::XMFLOAT3 get_position() const { return position; }
	const DirectX::XMFLOAT4 get_orientation() const { return orientation; }
	const DirectX::XMFLOAT3 get_scale() const { return scale; }
	const DirectX::XMFLOAT4X4 get_transform() const { return transform; }


private:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = {};
	Transform* parent_transform;
};