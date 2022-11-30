#include "instance_mesh_effect.h"
#include "noise.h"
#include "texture.h"
//==============================================================
// 
//�R���X�g���N�^
// 
//==============================================================
InstanceMeshEffect::InstanceMeshEffect(Graphics& graphics, const char* fbx_filename, const int max_instance)
{
	model = std::make_unique<InstanceMesh>(graphics, fbx_filename, max_instance);
	constants = std::make_unique<Constants<CONSTANTS>>(graphics.get_device().Get());
}

//==============================================================
// 
//�Đ�
// 
//==============================================================
void InstanceMeshEffect::play(DirectX::XMFLOAT3 pos)
{
	position = pos;
	active = true;
}

//==============================================================
// 
//��~
// 
//==============================================================
void InstanceMeshEffect::stop()
{
	
	active = false;
	life_time = 0;
}

//==============================================================
// 
//�X�V
// 
//==============================================================
void InstanceMeshEffect::update(Graphics& graphics, float elapsed_time)
{
	//�A�N�e�B�u��ԂȂ�
	if (active)
	{
		//�X�V����
		
		//��������
		life_time += elapsed_time;
		if (life_time > life_span)
		{
			if (is_loop)
			{
				life_time = 0;
			}
			else
			{
				stop();
			}
		}
	}
}
//==============================================================
// 
//�`��
// 
//==============================================================
void InstanceMeshEffect::render(Graphics& graphics)
{
	//�G�t�F�N�g���A�N�e�B�u��Ԃ̏ꍇ�̂ݕ`��
	if (!active) return;

	//�V�F�[�_�[���A�N�e�B�u��Ԃ�
	model->active(graphics.get_dc().Get(), material->get_ps().Get());
	//�萔�o�b�t�@���M
	constants->bind(graphics.get_dc().Get(), 9, CB_FLAG::PS_VS);
	//�V�F�[�_�[���\�[�X���M
	material->transfer_shader_resource_view(graphics);
	model->render(graphics);
}
//==============================================================
// 
//�f�o�b�OGUI
// 
//==============================================================
void InstanceMeshEffect::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "Param:" + str_id;
	/*�����艺�Ƀp�����[�^�[�L�q*/
	ImGui::DragFloat2("dir", &constants->data.scroll_direction.x, 0.1f);
	ImGui::DragFloat("speed", &constants->data.scroll_speed, 0.1f);
	ImGui::DragFloat3("rot_speed", &rot_speed.x);
	ImGui::DragFloat4("particle_color", &constants->data.particle_color.x, 0.1f);
	ImGui::DragFloat("threshold", &constants->data.threshold, 0.1f);
	ImGui::DragFloat("lifetime", &life_time, 0.1f);
	ImGui::DragFloat3("position", &position.x, 0.1f);
	ImGui::DragFloat3("scale", &scale.x, 0.1f);
	ImGui::Checkbox("play", &active);

	/*�������Ƀp�����[�^�[�L�q*/

#endif

}

//==============================================================
// 
// �p������]������
// 
//==============================================================
DirectX::XMFLOAT4 InstanceMeshEffect::set_rotate_quaternion(DirectX::XMFLOAT3 axis, float ang, int index)
{
	float angle = DirectX::XMConvertToRadians(ang);
	return Math::rot_quaternion(model->get_orientation(index), axis, angle);
}
//==============================================================
// 
// �����w��̌����ɍ��킹��
// 
//==============================================================
DirectX::XMFLOAT4 InstanceMeshEffect::rotate_base_axis(AXIS axis, DirectX::XMFLOAT3 dir_vec, int index)
{
	DirectX::XMFLOAT3 Axis;
	switch (axis)
	{
	case AXIS::RIGHT:
		Axis = Math::get_posture_right(model->get_orientation(index));
		return Math::rot_quaternion_dir(model->get_orientation(index), Axis, dir_vec);
		break;
	case AXIS::UP:
		Axis = Math::get_posture_up(model->get_orientation(index));
		return Math::rot_quaternion_dir(model->get_orientation(index), Axis, dir_vec);
		break;
	case AXIS::FORWARD:
		Axis = Math::get_posture_forward(model->get_orientation(index));
		return Math::rot_quaternion_dir(model->get_orientation(index), Axis, dir_vec);
		break;
	}
}
//==============================================================
// 
// ����C�ӂ̒l�ŉ�]������
// 
//==============================================================
DirectX::XMFLOAT4 InstanceMeshEffect::set_rotate_quaternion(AXIS axis, float ang, int index)
{
	float angle = DirectX::XMConvertToRadians(ang);
	DirectX::XMFLOAT3 Axis;
	switch (axis)
	{
	case AXIS::RIGHT:
		Axis = Math::get_posture_right(model->get_orientation(index));
		return Math::rot_quaternion(model->get_orientation(index), Axis, angle);
		break;
	case AXIS::UP:
		Axis = Math::get_posture_up(model->get_orientation(index));
		return Math::rot_quaternion(model->get_orientation(index), Axis, angle);
		break;
	case AXIS::FORWARD:
		Axis = Math::get_posture_forward(model->get_orientation(index));
		return Math::rot_quaternion(model->get_orientation(index), Axis, angle);
		break;
	}

}
