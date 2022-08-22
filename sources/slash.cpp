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
	load_texture_from_file(device, L"./resources/Effects/Slash/slash.png", shader_resource_view.ReleaseAndGetAddressOf(), &texture_desc);
	//load_texture_from_file(device, L"./resources/Sprite/kouboutest.png", shader_resource_view.ReleaseAndGetAddressOf(), &texture_desc);
	scale = { 0.1f, 0.1f, 0.1f };
}

void Slash::update(float elapsed_time)
{
	if (slash_timer > 0.12f)
	{
		slash_timer = 0;
		slash = false;
	}

	if (slash)
	{
		slash_timer += elapsed_time;
		orientation = Math::rot_quaternion(orientation, Math::get_posture_up(orientation), DirectX::XMConvertToRadians(-180), elapsed_time, 10);
	}
}

void Slash::render(Graphics& graphics)
{
#if USE_IMGUI
	imgui_menu_bar("Effects", "slash", display_slash_imgui);
	ImGui::Begin("slash");
	ImGui::DragFloat2("dir", &constance->data.scroll_direction.x, 0.1);
	ImGui::DragFloat("speed", &constance->data.scroll_speed, 0.1);
	ImGui::DragFloat3("position", &position.x, 0.1);
	ImGui::DragFloat3("scale", &scale.x, 0.1);
	ImGui::End();
#endif
	shader->active(graphics.get_dc().Get(), vertex_shader.Get(), pixel_shader.Get());
	//shader->active(graphics.get_dc().Get(), RENDER_TYPE::Forward);
	constance->bind(graphics.get_dc().Get(), 2, CB_FLAG::VS);
	DirectX::XMFLOAT4X4 world = Math::calc_world_matrix(scale, orientation, position);
	graphics.get_dc().Get()->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
	if(slash)shader->render(graphics.get_dc().Get(), model.get(), world);
}

void Slash::launch(DirectX::XMFLOAT3 pos, DirectX::XMVECTOR slash_dir_vec, DirectX::XMVECTOR slope_vec)
{
	position = pos;
	// XMVECTORÉNÉâÉXÇ÷ïœä∑
	DirectX::XMVECTOR orientationVec = DirectX::XMLoadFloat4(&orientation);
	if (DirectX::XMVector3Equal(slash_dir_vec, DirectX::XMVectorZero())) return; //Ç‡Çµmove_vecÇ™É[ÉçÉxÉNÉgÉãÇ»ÇÁÉäÉ^Å[Éì
	
	DirectX::XMVECTOR forward, up;
	DirectX::XMFLOAT3 axis;
	DirectX::XMVECTOR Axis;	//âÒì]é≤
	float angle;			//âÒì]äp
	DirectX::XMVECTOR Ang;

	//ê≥ãKâª
	slash_dir_vec = DirectX::XMVector3Normalize(slash_dir_vec);
	//Yé≤âÒì]
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

	//Zé≤âÒì]
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

	// orientationVecÇ©ÇÁorientationÇçXêV
	DirectX::XMStoreFloat4(&orientation, orientationVec);

	end_slash_vec = Math::rev_vec(Math::get_posture_right_vec(orientation));

	slash = true;
}
