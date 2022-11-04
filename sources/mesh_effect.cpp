#include "mesh_effect.h"
#include "user.h"
#include "texture.h"
//==============================================================
// 
// コンストラクタ
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
// 再生
// 
//==============================================================
void MeshEffect::play(DirectX::XMFLOAT3 pos)
{
	
	position = pos;
	active = true;
	orientation = Math::orientation_reset();
}
//==============================================================
// 
// 停止
// 
//==============================================================
void MeshEffect::stop()
{
	active = false;
	life_time = 0;
}
//==============================================================
// 
// 更新
// 
//==============================================================
void MeshEffect::update(Graphics& graphics, float elapsed_time)
{
	//アクティブ状態なら
	if (active)
	{
		//更新処理
		//****************************************
		//				回転処理
		//**************************************
		set_rotate_quaternion(AXIS::RIGHT, rot_speed.x * elapsed_time);
		set_rotate_quaternion(AXIS::UP, rot_speed.y * elapsed_time);
		set_rotate_quaternion(AXIS::FORWARD, rot_speed.z * elapsed_time);


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
#if USE_IMGUI
	if (display_imgui)
	{
		ImGui::Begin(instance_id.c_str());
		graphics.recompile_pixel_shader(pixel_shader.GetAddressOf(), instance_id.c_str());
		ImGui::End();
	}

#endif
}
//==============================================================
// 
// 描画
// 
//==============================================================
void MeshEffect::render(Graphics& graphics)
{
	//エフェクトがアクティブ状態の場合のみ描画
	if (!active) return;

	//ピクセルシェーダーが設定されていなければ警告
	_ASSERT_EXPR(pixel_shader, "ピクセルシェーダーが設定されていません");
	//シェーダーをアクティブ状態に
	shader->active(graphics.get_dc().Get(), vertex_shader.Get(), pixel_shader.Get());
	//定数バッファ送信
	constants->bind(graphics.get_dc().Get(), 9, CB_FLAG::PS_VS);
	//シェーダーリソース送信
	int resource_num = 0;
	const int send_texture_num = 1;
	for (auto& s : shader_resources)
	{
		graphics.get_dc().Get()->PSSetShaderResources(MATERIAL_START_SLOT + resource_num, send_texture_num, s.GetAddressOf());
		resource_num++;
	}
	//トランスフォーム更新
	transform = Math::calc_world_matrix(scale, orientation, position);
	//レンダー
	shader->render(graphics.get_dc().Get(), model.get(), transform);

	
}
//==============================================================
// 
// 姿勢を回転させる
// 
//==============================================================
void MeshEffect::set_rotate_quaternion(DirectX::XMFLOAT3 axis, float ang)
{
	float angle = DirectX::XMConvertToRadians(ang);
	orientation = Math::rot_quaternion(orientation, axis, angle);
}
//==============================================================
// 
// 軸を指定の向きに合わせる
// 
//==============================================================
void MeshEffect::rotate_base_axis(AXIS axis, DirectX::XMFLOAT3 dir_vec)
{
	DirectX::XMFLOAT3 Axis;
	switch (axis)
	{
	case AXIS::RIGHT:
		Axis = Math::get_posture_right(orientation);
		orientation = Math::rot_quaternion_dir(orientation, Axis, dir_vec);
		break;
	case AXIS::UP:
		Axis = Math::get_posture_up(orientation);
		orientation = Math::rot_quaternion_dir(orientation, Axis, dir_vec);
		break;
	case AXIS::FORWARD:
		Axis = Math::get_posture_forward(orientation);
		orientation = Math::rot_quaternion_dir(orientation, Axis, dir_vec);
		break;
	default:
		break;
	}
}
//==============================================================
// 
// 軸を任意の値で回転させる
// 
//==============================================================
void MeshEffect::set_rotate_quaternion(AXIS axis, float ang)
{
	float angle = DirectX::XMConvertToRadians(ang);
	DirectX::XMFLOAT3 Axis;
	switch (axis)
	{
	case AXIS::RIGHT:
		Axis = Math::get_posture_right(orientation);
		orientation = Math::rot_quaternion(orientation, Axis, angle);
		break;
	case AXIS::UP:
		Axis = Math::get_posture_up(orientation);
		orientation = Math::rot_quaternion(orientation, Axis, angle);
		break;
	case AXIS::FORWARD:
		Axis = Math::get_posture_forward(orientation);
		orientation = Math::rot_quaternion(orientation, Axis, angle);
		break;
	default:
		break;
	}

}
//==============================================================
// 
//リソースに登録（シェーダーリソースビューを受け取る）
// 
//==============================================================
void MeshEffect::register_shader_resource(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	shader_resources.push_back(srv);
}
//==============================================================
// 
//リソースに登録（テクスチャのファイル名を受け取る）
// 
//==============================================================
void MeshEffect::register_shader_resource(ID3D11Device* device, const wchar_t* filename)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	load_texture_from_file(device, filename, shader_resource_view.ReleaseAndGetAddressOf(), &texture2d_desc);

	shader_resources.push_back(shader_resource_view);
}
//==============================================================
// 
//ピクセルシェーダー生成
// 
//==============================================================
void MeshEffect::create_pixel_shader(ID3D11Device* device, const char* cso_name)
{
	create_ps_from_cso(device, cso_name, pixel_shader.ReleaseAndGetAddressOf());
}
//==============================================================
// 
//姿勢リセット
// 
//==============================================================
void MeshEffect::reset_orientation()
{
	orientation = Math::orientation_reset();
}
//==============================================================
// 
//デバッグGUI
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
		/*これより下にパラメーター記述*/
		if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat2("dir", &constants->data.scroll_direction.x, 0.1f);
			ImGui::DragFloat("speed", &constants->data.scroll_speed, 0.1f);
			ImGui::DragFloat3("rot_speed", &rot_speed.x);
			ImGui::DragFloat4("particle_color", &constants->data.particle_color.x, 0.1f);
			ImGui::DragFloat("threshold", &constants->data.threshold, 0.1f);
			ImGui::DragFloat("lifetime", &life_time, 0.1f);
			ImGui::DragFloat3("position", &position.x, 0.1f);
			ImGui::DragFloat3("scale", &scale.x, 0.1f);
			ImGui::Checkbox("play", &active);

		}
		/*これより上にパラメーター記述*/
		ImGui::PopID();
		ImGui::End();
	}
#endif
}
