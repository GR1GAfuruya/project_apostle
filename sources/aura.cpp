#include "aura.h"
#include "user.h"

#include "texture.h"
#include "user.h"
Aura::Aura(ID3D11Device* device)
{
	model = make_unique<SkeletalMesh>(device, "./resources/Effects/Meshes/eff_spiral.fbx");
	shader = make_unique<MeshShader>(device);

	create_vs_from_cso(device, "shaders/aura_vs.cso",
		vertex_shader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	create_ps_from_cso(device, "shaders/aura_ps.cso", pixel_shader.ReleaseAndGetAddressOf());
    constants = std::make_unique<Constants<AURA_CONSTANTS>>(device);
    D3D11_TEXTURE2D_DESC texture2d_desc{};
	load_texture_from_file(device, L"./resources/Effects/Slash/slash.png", shader_resource_views[0].ReleaseAndGetAddressOf(), &texture2d_desc);
	load_texture_from_file(device, L"./resources/TexMaps/Mask/dissolve_animation.png", shader_resource_views[1].ReleaseAndGetAddressOf(), &texture2d_desc);

}


void Aura::play(DirectX::XMFLOAT3 pos)
{
	position = pos;
	active = true;
	orientation = Math::orientation_reset();
}

void Aura::stop()
{
	EffecttBase::stop();
	constants->data.scroll_speed = 0;
	orientation = Math::orientation_reset();
}

void Aura::update(Graphics& graphics,float elapsed_time)
{
	//アクティブ状態なら
	if (active)
	{
		//更新処理
		set_rotate_quaternion(AXIS::FORWARD, rot_speed * elapsed_time);
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

void Aura::render(Graphics& graphics)
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

void Aura::debug_gui(const char* str_id)
{
#if USE_IMGUI
	string name = "auras:" + to_string(*str_id);
	imgui_menu_bar("Effects", name, display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("aura");
		ImGui::PushID(str_id);
		/*これより下にパラメーター記述*/

		ImGui::DragFloat2("dir", &constants->data.scroll_direction.x, 0.1);
		ImGui::DragFloat("speed", &constants->data.scroll_speed, 0.1);
		ImGui::DragFloat("rot_speed", &rot_speed);
		ImGui::DragFloat4("particle_color", &constants->data.particle_color.x, 0.1);
		ImGui::DragFloat("threshold", &constants->data.threshold, 0.1);
		ImGui::DragFloat3("position", &position.x, 0.1);
		ImGui::DragFloat3("scale", &scale.x, 0.1);
		ImGui::Checkbox("play", &active);

		/*これより上にパラメーター記述*/
		ImGui::PopID();
		ImGui::End();
	}
#endif
}
