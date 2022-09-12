#include "sword.h"
#include "user.h"
Sword::Sword(Graphics& graphics)
{
	model = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), "./resources/Model/Player/dark_sword.fbx");
	scale = { 0.008f,0.008f,0.008f };
}

void Sword::initialize()
{
}

void Sword::update(Graphics& graphics, float elapsed_time)
{
}

void Sword::render(Graphics &graphics)
{
	transform = Math::calc_world_matrix(scale, orientation, position);
	graphics.shader->render(graphics.get_dc().Get(), model.get(), transform);
}

void Sword::set_sword_dir(const DirectX::XMFLOAT3 dir)
{
	//orientation = Math::orientation_reset();
	orientation = Math::rot_quaternion_dir(orientation, Math::get_posture_up(orientation), dir);
}
