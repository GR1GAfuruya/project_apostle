#include "sword.h"
#include "user.h"
Sword::Sword(Graphics& graphics)
{
	model = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), "./resources/Model/Player/dark_sword.fbx");
	scale = { 0.17f,-0.17f,0.17f };
	position = { -7.850f,-3.88f, 15.6f };
}

void Sword::initialize()
{
}

void Sword::update(Graphics& graphics, float elapsed_time)
{
}

void Sword::render(Graphics &graphics)
{
	transform = Math::calc_world_matrix(parent_transform,scale, orientation, position);
	graphics.shader->render(graphics.get_dc().Get(), model.get(), transform);
#if USE_IMGUI
	ImGui::Begin("sword");
	ImGui::DragFloat3("scale", &scale.x, 0.01f);
	ImGui::DragFloat3("position", &position.x, 0.01f);
	ImGui::DragFloat4("ori", &orientation.x, 0.01f);
	ImGui::End();
#endif
}

void Sword::set_sword_dir(const DirectX::XMFLOAT3 dir)
{
	//orientation = Math::orientation_reset();
	orientation = Math::rot_quaternion_dir(orientation, Math::get_posture_up(orientation), dir);
}
