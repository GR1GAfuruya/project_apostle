#include "tornado.h"
#include "user.h"

#include "texture.h"
#include "user.h"
Tornado::Tornado(ID3D11Device* device)
{
	model = make_unique<SkeletalMesh>(device, "./resources/Effects/Meshes/eff_tornado.fbx");
	shader = make_unique<MeshShader>(device);

	create_vs_from_cso(device, "shaders/aura_vs.cso",
		vertex_shader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	create_ps_from_cso(device, "shaders/fire_ps.cso", pixel_shader.ReleaseAndGetAddressOf());
	constants = std::make_unique<Constants<CONSTANTS>>(device);
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	load_texture_from_file(device, L"./resources/Effects/Textures/Traill2_output.png", shader_resource_views[0].ReleaseAndGetAddressOf(), &texture2d_desc);
	load_texture_from_file(device, L"./resources/TexMaps/Mask/dissolve_animation.png", shader_resource_views[1].ReleaseAndGetAddressOf(), &texture2d_desc);

}


void Tornado::play(DirectX::XMFLOAT3 pos)
{
	position = pos;
	active = true;
	orientation = Math::orientation_reset();
}

void Tornado::stop()
{
	EffecttBase::stop();
	constants->data.scroll_speed = 0;
	orientation = Math::orientation_reset();
}

void Tornado::update(Graphics& graphics, float elapsed_time)
{
	//アクティブ状態なら
	if (active)
	{
		//更新処理
		orientation = Math::rot_quaternion(orientation, Math::get_posture_forward(orientation), DirectX::XMConvertToRadians(rot_speed * elapsed_time));
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

void Tornado::render(Graphics& graphics)
{

	//エフェクトがアクティブ状態の場合のみ描画
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

void Tornado::debug_gui(const char* str_id)
{

#if USE_IMGUI
	imgui_menu_bar("Effects", "tornads", display_imgui);
	if (display_imgui)
	{
		string name = "tornad:" + to_string(*str_id);
		ImGui::Begin(name.c_str());
		ImGui::PushID(str_id);
		ImGui::DragFloat2("dir", &constants->data.scroll_direction.x, 0.1);
		ImGui::DragFloat("speed", &constants->data.scroll_speed, 0.1);
		ImGui::DragFloat("rot_speed", &rot_speed);
		ImGui::DragFloat4("particle_color", &constants->data.particle_color.x, 0.1);
		ImGui::DragFloat("threshold", &constants->data.threshold, 0.1f,0.0f,1.0f);
		ImGui::DragFloat3("position", &position.x, 0.1);
		ImGui::DragFloat3("scale", &scale.x, 0.1);
		ImGui::Checkbox("play", &active);
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::End();
	}
#endif
}
