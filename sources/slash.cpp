#include "slash.h"
#include "user.h"
#include "texture.h"
Slash::Slash(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	model = make_unique<SkeletalMesh>(device, "./resources/Model/SlashMesh.fbx");
	shader = make_unique<MeshShader>(device);
	//shader = make_unique<LambertShader>(device);
	constants = make_unique<Constants<SlashConstance>>(device);
	hr = create_vs_from_cso(device, "shaders/slash_vs.cso", vertex_shader.ReleaseAndGetAddressOf(),
		nullptr, nullptr, 0);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = create_ps_from_cso(device, "shaders/slash_ps.cso", pixel_shader.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_TEXTURE2D_DESC texture_desc{};
	//load_texture_from_file(device, L"./resources/Effects/Slash/slash.png", shader_resource_views[0].ReleaseAndGetAddressOf(), &texture_desc);
	load_texture_from_file(device, L"./resources/Effects/Textures/Traill3_output.png", shader_resource_views[1].ReleaseAndGetAddressOf(), &texture_desc);
	scale = { 0.1f, 0.1f, 0.1f };
	life_span = 0.14;
}



void Slash::stop()
{
	EffecttBase::stop();
}

void Slash::update(Graphics& graphics, float elapsed_time)
{
	if (active)
	{
		life_time += elapsed_time;
		if (dir)
		{
			orientation = Math::rot_quaternion(orientation, Math::get_posture_up(orientation), -30, elapsed_time, rotate_rate);
		}
		else
		{
			orientation = Math::rot_quaternion(orientation, Math::get_posture_up(orientation), 30, elapsed_time, rotate_rate);
		}

		//寿命処理
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

void Slash::render(Graphics& graphics)
{

	if (!active) return;
	
	//シェーダーをアクティブ状態に
	shader->active(graphics.get_dc().Get(), vertex_shader.Get(), pixel_shader.Get());
	//定数バッファ送信
	constants->bind(graphics.get_dc().Get(), 9, CB_FLAG::PS_VS);
	graphics.get_dc().Get()->PSSetShaderResources(20, 1, shader_resource_views[0].GetAddressOf());
	graphics.get_dc().Get()->PSSetShaderResources(21, 1, shader_resource_views[1].GetAddressOf());
	//if(slash)
	DirectX::XMFLOAT4X4 world = Math::calc_world_matrix(scale, orientation, position);
	shader->render(graphics.get_dc().Get(), model.get(), world);
}

void Slash::debug_gui(const char* str_id)
{
#if USE_IMGUI
	imgui_menu_bar("Effects", "slash", display_slash_imgui);
	ImGui::Begin("slash");
	ImGui::PushID(str_id);
	/*これより下にパラメーター記述*/
	ImGui::DragFloat2("dir", &constants->data.scroll_direction.x, 0.1);
	ImGui::DragFloat("speed", &constants->data.scroll_speed, 0.1);
	ImGui::DragFloat4("ori", &orientation.x, 0.1);
	ImGui::DragFloat4("particle_color", &constants->data.particle_color.x, 0.1);
	ImGui::DragFloat3("position", &position.x, 0.1);
	ImGui::DragFloat3("scale", &scale.x, 0.1);
	/*これより上にパラメーター記述*/
	ImGui::PopID();
	ImGui::End();
#endif
}

void Slash::play(DirectX::XMFLOAT3 pos)
{
	active = true;
	position = pos;
	orientation = Math::orientation_reset();
}

void Slash::play(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 slash_dir_vec, float tilt_degree, bool direction)
{
	//位置設定
	position = pos;

	active = true;
	//基準軸のリセット
	orientation = Math::orientation_reset();
	
	DirectX::XMVECTOR right, up;
	float angle;			//回転角
	DirectX::XMVECTOR Ang;
	//正規化
	DirectX::XMVECTOR Dir = DirectX::XMLoadFloat3(&slash_dir_vec);
	//Y軸回転
	{
		right =Math::rev_vec_v( Math::get_posture_right_vec(orientation));
		Ang = DirectX::XMVector3Dot(right, Dir);
		DirectX::XMStoreFloat(&angle, Ang);
		angle = acosf(angle);
		set_rotate_quaternion(AXIS::UP, DirectX::XMConvertToDegrees(angle));
	}
	//X軸回転
	{
		set_rotate_quaternion(AXIS::RIGHT, tilt_degree);
	}
	dir = direction;
}
