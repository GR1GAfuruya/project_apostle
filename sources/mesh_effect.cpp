#include "mesh_effect.h"
#include "user.h"
#include "texture.h"
//==============================================================
// 
// �R���X�g���N�^
// 
//==============================================================
MeshEffect::MeshEffect(Graphics& graphics, const char* fbx_filename)
{
	model = make_unique<SkeletalMesh>(graphics.get_device().Get(), fbx_filename);
	constants = std::make_unique<Constants<CONSTANTS>>(graphics.get_device().Get());
	create_vs_from_cso(graphics.get_device().Get(), "shaders/mesh_effect_vs.cso",
		vertex_shader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);

	shader = make_unique<MeshShader>(graphics.get_device().Get());
}
//==============================================================
// 
// �Đ�
// 
//==============================================================
void MeshEffect::play(DirectX::XMFLOAT3 pos)
{
	//���������K��
	constants->data.threshold = 0.0f;
	rot_speed = { 0,0,0 };
	init_effect_param.position = pos;
	init_effect_param.orientation = Math::orientation_reset();
	effect_param = init_effect_param;
	active = true;
}
//==============================================================
// 
// ��~
// 
//==============================================================
void MeshEffect::stop()
{
	active = false;
	life_time = 0;
}
//==============================================================
// 
// �X�V
// 
//==============================================================
void MeshEffect::update(Graphics& graphics, float elapsed_time)
{
	//�A�N�e�B�u��ԂȂ�
	if (active)
	{
		//�X�V����
		//****************************************
		//				��]����
		//**************************************
		set_rotate_quaternion(AXIS::RIGHT, rot_speed.x * elapsed_time);
		set_rotate_quaternion(AXIS::UP, rot_speed.y * elapsed_time);
		set_rotate_quaternion(AXIS::FORWARD, rot_speed.z * elapsed_time);

		dissolve_update(elapsed_time);

		constants->data.particle_color = effect_param.color;

		//��������
		life_time += elapsed_time;
		if (life_time > effect_param.life_duration)
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
// �`�� ���t���X�^���J�����O����
// 
//==============================================================
void MeshEffect::render(Graphics& graphics)
{
	//�G�t�F�N�g���A�N�e�B�u��Ԃ̏ꍇ�̂ݕ`��
	if (!active) return;

	//�s�N�Z���V�F�[�_�[���ݒ肳��Ă��Ȃ���Όx��
	//_ASSERT_EXPR(pixel_shader, "�s�N�Z���V�F�[�_�[���ݒ肳��Ă��܂���");
	//�V�F�[�_�[���A�N�e�B�u��Ԃ�
	shader->active(graphics.get_dc().Get(), vertex_shader.Get(), material->get_ps().Get());
	//�萔�o�b�t�@���M
	constants->bind(graphics.get_dc().Get(), 9, CB_FLAG::PS_VS);
	//�V�F�[�_�[���\�[�X���M
	material->transfer_shader_resource_view(graphics);
	//�g�����X�t�H�[���X�V
	transform = Math::calc_world_matrix(effect_param.scale, effect_param.orientation, effect_param.position);
	//�����_�[
	shader->render(graphics.get_dc().Get(), model.get(), transform);


}
//==============================================================
// 
// �`�� ���t���X�^���J�����O����
// 
//==============================================================
void MeshEffect::render(Graphics& graphics,Camera* camera)
{
	//�G�t�F�N�g���A�N�e�B�u��Ԃ̏ꍇ�̂ݕ`��
	if (!active) return;

	//�s�N�Z���V�F�[�_�[���ݒ肳��Ă��Ȃ���Όx��
	//_ASSERT_EXPR(pixel_shader, "�s�N�Z���V�F�[�_�[���ݒ肳��Ă��܂���");
	//�V�F�[�_�[���A�N�e�B�u��Ԃ�
	shader->active(graphics.get_dc().Get(), vertex_shader.Get(), material->get_ps().Get());
	//�萔�o�b�t�@���M
	constants->bind(graphics.get_dc().Get(), 9, CB_FLAG::PS_VS);
	//�V�F�[�_�[���\�[�X���M
	material->transfer_shader_resource_view(graphics);
	//�g�����X�t�H�[���X�V
	transform = Math::calc_world_matrix(effect_param.scale, effect_param.orientation, effect_param.position);
	//�����_�[
	shader->render(graphics.get_dc().Get(), model.get(),camera->get_view(),camera->get_projection(), transform);

	
}
//==============================================================
// 
// �p������]������
// 
//==============================================================
void MeshEffect::set_rotate_quaternion(DirectX::XMFLOAT3 axis, float ang)
{
	float angle = DirectX::XMConvertToRadians(ang);
	effect_param.orientation = Math::rot_quaternion(effect_param.orientation, axis, angle);
}
//==============================================================
// 
// �����w��̌����ɍ��킹��
// 
//==============================================================
void MeshEffect::rotate_base_axis(AXIS axis, DirectX::XMFLOAT3 dir_vec)
{
	DirectX::XMFLOAT3 Axis;
	switch (axis)
	{
	case AXIS::RIGHT:
		Axis = Math::get_posture_right(effect_param.orientation);
		effect_param.orientation = Math::rot_quaternion_dir(effect_param.orientation, Axis, dir_vec);
		break;
	case AXIS::UP:
		Axis = Math::get_posture_up(effect_param.orientation);
		effect_param.orientation = Math::rot_quaternion_dir(effect_param.orientation, Axis, dir_vec);
		break;
	case AXIS::FORWARD:
		Axis = Math::get_posture_forward(effect_param.orientation);
		effect_param.orientation = Math::rot_quaternion_dir(effect_param.orientation, Axis, dir_vec);
		break;
	default:
		break;
	}
}
//==============================================================
// 
// ����C�ӂ̒l�ŉ�]������
// 
//==============================================================
void MeshEffect::set_rotate_quaternion(AXIS axis, float ang)
{
	float angle = DirectX::XMConvertToRadians(ang);
	DirectX::XMFLOAT3 Axis;
	switch (axis)
	{
	case AXIS::RIGHT:
		Axis = Math::get_posture_right(effect_param.orientation);
		effect_param.orientation = Math::rot_quaternion(effect_param.orientation, Axis, angle);
		break;
	case AXIS::UP:
		Axis = Math::get_posture_up(effect_param.orientation);
		effect_param.orientation = Math::rot_quaternion(effect_param.orientation, Axis, angle);
		break;
	case AXIS::FORWARD:
		Axis = Math::get_posture_forward(effect_param.orientation);
		effect_param.orientation = Math::rot_quaternion(effect_param.orientation, Axis, angle);
		break;
	default:
		break;
	}

}
//==============================================================
// 
//�p�����Z�b�g
// 
//==============================================================
void MeshEffect::reset_orientation()
{
	effect_param.orientation = Math::orientation_reset();
}

//==============================================================
// 
//�f�B�]���u�X�V
// 
//==============================================================
void MeshEffect::dissolve_update(float elapsed_time)
{
	//�t�F�[�h�C��
	
	//�t�F�[�h�A�E�g
	const float dissolve_rate = life_time / effect_param.life_duration;
	//�f�B�]���u����
	if (life_time < effect_param.life_duration)
	{
		constants->data.threshold = lerp(constants->data.threshold, 0.0f, dissolve_rate);
		constants->data.threshold = lerp(constants->data.threshold, 1.0f, dissolve_rate);
	}

	//�f�B�]���u�̃N�����v
	if (constants->data.threshold > 1.0f)
	{
		constants->data.threshold = 1.0f;
	}

}
//==============================================================
// 
//�f�o�b�OGUI
// 
//==============================================================
void MeshEffect::debug_gui(string str_id)
{
#if USE_IMGUI
	instance_id = str_id;
	imgui_menu_bar("Effects", str_id, display_imgui);
	if (display_imgui)
	{
		string name = "Param:" + str_id;
		ImGui::Begin(str_id.c_str());
		ImGui::PushID(str_id.c_str());
		/*�����艺�Ƀp�����[�^�[�L�q*/
		if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat2("dir", &constants->data.scroll_direction.x, 0.1f);
			ImGui::DragFloat("speed", &constants->data.scroll_speed, 0.1f);
			ImGui::DragFloat3("rot_speed", &rot_speed.x);
			ImGui::DragFloat4("particle_color", &constants->data.particle_color.x, 0.1f);
			ImGui::DragFloat("threshold", &constants->data.threshold, 0.1f);
			ImGui::DragFloat("lifetime", &life_time, 0.1f);
			ImGui::DragFloat3("position", &effect_param.position.x, 0.1f);
			ImGui::DragFloat3("scale", &effect_param.scale.x, 0.1f);
			ImGui::Checkbox("play", &active);

		}
		/*�������Ƀp�����[�^�[�L�q*/
		ImGui::PopID();
		ImGui::End();
	}
#endif
}
