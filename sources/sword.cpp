#include "sword.h"
#include "user.h"
#include "Operators.h"
Sword::Sword(Graphics& graphics)
{
	model = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), "./resources/Model/Player/dark_sword.fbx");
	
}

void Sword::initialize()
{
	scale = { 0.17f,-0.17f,0.17f };
	position = { -7.850f,-3.88f, 15.6f };
	//剣の長さ
	length = 5.5f;
	//剣の太さ
	radius = 0.6f;
}

void Sword::update(Graphics& graphics, float elapsed_time)
{
	//ワールド行列更新
	transform = Math::calc_world_matrix(parent_transform, scale, orientation, position);
	//当たり判定情報更新
	DirectX::XMFLOAT3 up = get_sword_end_vec();
	//位置は装備したときの剣の位置
	collision.start = get_equipped_position();
	collision.end = get_equipped_position() + Math::vector_scale(up, length);
	collision.radius = radius;
}

void Sword::render(Graphics &graphics)
{
	graphics.shader->render(graphics.get_dc().Get(), model.get(), transform);
#if USE_IMGUI
	ImGui::Begin("sword");
	ImGui::DragFloat3("scale", &scale.x, 0.01f);
	ImGui::DragFloat3("position", &position.x, 0.01f);
	ImGui::DragFloat4("ori", &orientation.x, 0.01f);
	ImGui::DragFloat3("length", &length, 0.01f);
	ImGui::DragFloat3("radius", &radius, 0.01f);
	ImGui::End();
#endif
}


const DirectX::XMFLOAT3& Sword::get_sword_end_vec() const
{
	DirectX::XMFLOAT4X4 w = transform;

	DirectX::XMFLOAT3 pos = { w._41,w._42,w._43 };
	DirectX::XMFLOAT3 scale = { Math::Length({w._11,w._12,w._13}),  Math::Length({w._21,w._22,w._23}),  Math::Length({w._31,w._32,w._33}) };

	DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) };
	DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z) };
	DirectX::XMMATRIX R = DirectX::XMLoadFloat4x4(&w) * DirectX::XMMatrixInverse(nullptr, S) * DirectX::XMMatrixInverse(nullptr, T);

	DirectX::XMFLOAT4X4 r;
	DirectX::XMStoreFloat4x4(&r, R);

	return { r._31,r._32, r._33 };
}

void Sword::set_sword_dir(const DirectX::XMFLOAT3 dir)
{
	//orientation = Math::orientation_reset();
	orientation = Math::rot_quaternion_dir(orientation, Math::get_posture_up(orientation), dir);
}
