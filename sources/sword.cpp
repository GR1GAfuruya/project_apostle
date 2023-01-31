#include "sword.h"

#include "Operators.h"
Sword::Sword(Graphics& graphics)
{
	model = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), "./resources/Model/Player/dark_sword.fbx");
	collision = {};
}

void Sword::initialize()
{
	scale = { 0.17f,-0.17f,0.17f };
	position = { -7.850f,-3.88f, 15.6f };
	//���̒���
	length = 7.5f;
	//���̑���
	radius = 0.6f;
}

void Sword::update(Graphics& graphics, float elapsed_time)
{
	//���[���h�s��X�V
	transform = Math::calc_world_matrix(parent_transform, scale, orientation, position);
	//�����蔻����X�V
	DirectX::XMFLOAT3 forward = get_dir_sword_top();
	//�ʒu�͑��������Ƃ��̌��̈ʒu
	collision.start = get_equipped_position();
	collision.end = get_equipped_position() + Math::vector_scale(forward, length);
	collision.radius = radius;
}

void Sword::render(Graphics &graphics)
{
	graphics.shader->render(graphics.get_dc().Get(), model.get(), transform);
#if USE_IMGUI
	imgui_menu_and_sub_bar("Charactor", "wepon", "sword", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("sword");
		ImGui::DragFloat3("scale", &scale.x, 0.01f);
		ImGui::DragFloat3("position", &position.x, 0.01f);
		ImGui::DragFloat4("ori", &orientation.x, 0.01f);
		ImGui::DragFloat3("length", &length, 0.01f);
		ImGui::DragFloat3("radius", &radius, 0.01f);
		ImGui::End();
	}
#endif
}
