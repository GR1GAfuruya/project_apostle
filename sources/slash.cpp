#include "slash.h"
#include "user.h"
#include "texture.h"
Slash::Slash(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	model = make_unique<SkeletalMesh>(device, "./resources/Model/SlashMesh.fbx");
	shader = make_unique<MeshShader>(device);
	//shader = make_unique<LambertShader>(device);
	constance = make_unique<Constants<SlashConstance>>(device);
	hr = create_vs_from_cso(device, "shaders/slash_vs.cso", vertex_shader.ReleaseAndGetAddressOf(),
		nullptr, nullptr, 0);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = create_ps_from_cso(device, "shaders/slash_ps.cso", pixel_shader.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_TEXTURE2D_DESC texture_desc{};
	load_texture_from_file(device, L"./resources/Effects/Slash/slash.png", shader_resource_views[0].ReleaseAndGetAddressOf(), &texture_desc);
	load_texture_from_file(device, L"./resources/Effects/Textures/Traill3_output.png", shader_resource_views[1].ReleaseAndGetAddressOf(), &texture_desc);
	scale = { 0.1f, 0.1f, 0.1f };
}

void Slash::update(Graphics& graphics, float elapsed_time)
{
	if (slash_timer >= SLASH_MAX_TIME)
	{
		slash_timer = 0;
		slash = false;
	}

	float alpha = 0.0f;
	if (slash)
	{
		alpha = fabsf((cosf(slash_timer / SLASH_MAX_TIME)));
		slash_timer += elapsed_time;
		if (dir)
		{
			orientation = Math::rot_quaternion(orientation, Math::get_posture_up(orientation), -60, elapsed_time, 10);
		}
		else
		{
			orientation = Math::rot_quaternion(orientation, Math::get_posture_up(orientation), 60, elapsed_time, 10);
		}
	}
		constance->data.particle_color.w = Math::clamp(alpha,0.0f,1.0f);
}

void Slash::render(Graphics& graphics)
{
#if USE_IMGUI
	imgui_menu_bar("Effects", "slash", display_slash_imgui);
	ImGui::Begin("slash");
	ImGui::DragFloat2("dir", &constance->data.scroll_direction.x, 0.1);
	ImGui::DragFloat("speed", &constance->data.scroll_speed, 0.1);
	ImGui::DragFloat4("particle_color", &constance->data.particle_color.x, 0.1);
	ImGui::DragFloat3("position", &position.x, 0.1);
	ImGui::DragFloat3("scale", &scale.x, 0.1);
	ImGui::End();
#endif
	//シェーダーをアクティブ状態に
	shader->active(graphics.get_dc().Get(), vertex_shader.Get(), pixel_shader.Get());
	//定数バッファ送信
	constance->bind(graphics.get_dc().Get(), 2, CB_FLAG::PS_VS);
	graphics.get_dc().Get()->PSSetShaderResources(20, 1, shader_resource_views[0].GetAddressOf());
	graphics.get_dc().Get()->PSSetShaderResources(21, 1, shader_resource_views[1].GetAddressOf());
	//if(slash)
	DirectX::XMFLOAT4X4 world = Math::calc_world_matrix(scale, orientation, position);
	shader->render(graphics.get_dc().Get(), model.get(), world);
}

void Slash::play(DirectX::XMFLOAT3 pos, DirectX::XMVECTOR slash_dir_vec, DirectX::XMVECTOR slope_vec, bool direction)
{
	position = pos;
	// XMVECTORクラスへ変換
	DirectX::XMVECTOR orientationVec = DirectX::XMLoadFloat4(&orientation);
	if (DirectX::XMVector3Equal(slash_dir_vec, DirectX::XMVectorZero())) return; //もしmove_vecがゼロベクトルならリターン
	
	DirectX::XMVECTOR forward, up;
	DirectX::XMFLOAT3 axis;
	DirectX::XMVECTOR Axis;	//回転軸
	float angle;			//回転角
	DirectX::XMVECTOR Ang;

	//正規化
	slash_dir_vec = DirectX::XMVector3Normalize(slash_dir_vec);
	//Y軸回転
	{
		up = Math::get_posture_up_vec(orientation);
		forward = Math::get_posture_forward_vec(orientation);
		up = DirectX::XMVector3Normalize(up);
		forward = DirectX::XMVector3Normalize(forward);


		Axis = up;
		DirectX::XMStoreFloat3(&axis, Axis);
		DirectX::XMVECTOR Ang = DirectX::XMVector3Dot(forward, slash_dir_vec);
		DirectX::XMStoreFloat(&angle, Ang);
		angle = acosf(angle);

		orientation = Math::rot_quaternion(orientation, axis, angle);
	}

	//Z軸回転
	{
		up = Math::get_posture_up_vec(orientation);
		forward = Math::get_posture_forward_vec(orientation);
		up = DirectX::XMVector3Normalize(up);
		forward = DirectX::XMVector3Normalize(forward);
		
		Axis = forward;
		DirectX::XMStoreFloat3(&axis, Axis);
		Ang = DirectX::XMVector3Dot(up, slope_vec);
		DirectX::XMStoreFloat(&angle, Ang);
		angle = acosf(angle);

		orientation = Math::rot_quaternion(orientation, axis, angle);
	}
	dir = direction;
	slash = true;
}
