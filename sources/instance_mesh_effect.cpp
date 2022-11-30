#include "instance_mesh_effect.h"
#include "noise.h"
#include "texture.h"
//==============================================================
// 
//コンストラクタ
// 
//==============================================================
InstanceMeshEffect::InstanceMeshEffect(Graphics& graphics, const char* fbx_filename, const int max_instance)
{
	model = std::make_unique<InstanceMesh>(graphics, fbx_filename, max_instance);
	constants = std::make_unique<Constants<CONSTANTS>>(graphics.get_device().Get());
}

//==============================================================
// 
//再生
// 
//==============================================================
void InstanceMeshEffect::play(DirectX::XMFLOAT3 pos)
{
	position = pos;
	active = true;
}

//==============================================================
// 
//停止
// 
//==============================================================
void InstanceMeshEffect::stop()
{
	
	active = false;
	life_time = 0;
}

//==============================================================
// 
//更新
// 
//==============================================================
void InstanceMeshEffect::update(Graphics& graphics, float elapsed_time)
{
	//アクティブ状態なら
	if (active)
	{
		//更新処理
		
		//寿命処理
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
//描画
// 
//==============================================================
void InstanceMeshEffect::render(Graphics& graphics)
{
	//エフェクトがアクティブ状態の場合のみ描画
	if (!active) return;

	//シェーダーをアクティブ状態に
	model->active(graphics.get_dc().Get(), material->get_ps().Get());
	//定数バッファ送信
	constants->bind(graphics.get_dc().Get(), 9, CB_FLAG::PS_VS);
	//シェーダーリソース送信
	material->transfer_shader_resource_view(graphics);
	model->render(graphics);
}
//==============================================================
// 
//デバッグGUI
// 
//==============================================================
void InstanceMeshEffect::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "Param:" + str_id;
	/*これより下にパラメーター記述*/
	ImGui::DragFloat2("dir", &constants->data.scroll_direction.x, 0.1f);
	ImGui::DragFloat("speed", &constants->data.scroll_speed, 0.1f);
	ImGui::DragFloat3("rot_speed", &rot_speed.x);
	ImGui::DragFloat4("particle_color", &constants->data.particle_color.x, 0.1f);
	ImGui::DragFloat("threshold", &constants->data.threshold, 0.1f);
	ImGui::DragFloat("lifetime", &life_time, 0.1f);
	ImGui::DragFloat3("position", &position.x, 0.1f);
	ImGui::DragFloat3("scale", &scale.x, 0.1f);
	ImGui::Checkbox("play", &active);

	/*これより上にパラメーター記述*/

#endif

}

//==============================================================
// 
// 姿勢を回転させる
// 
//==============================================================
DirectX::XMFLOAT4 InstanceMeshEffect::set_rotate_quaternion(DirectX::XMFLOAT3 axis, float ang, int index)
{
	float angle = DirectX::XMConvertToRadians(ang);
	return Math::rot_quaternion(model->get_orientation(index), axis, angle);
}
//==============================================================
// 
// 軸を指定の向きに合わせる
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
// 軸を任意の値で回転させる
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
