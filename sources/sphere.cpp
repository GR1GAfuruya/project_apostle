#include "sphere.h"
#include "texture.h"

Sphere::Sphere(ID3D11Device* device)
{
	model = make_unique<SkeletalMesh>(device, "./resources/Effects/Meshes/eff_sphere.fbx");
	shader = make_unique<MeshShader>(device);

	create_vs_from_cso(device, "shaders/aura_vs.cso",
		vertex_shader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	create_ps_from_cso(device, "shaders/core_ps.cso", pixel_shader.ReleaseAndGetAddressOf());
	constants = std::make_unique<Constants<SPHERE_CONSTANTS>>(device);
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	load_texture_from_file(device, L"./resources/Effects/Textures/Traill2_output.png", textures[0].ReleaseAndGetAddressOf(), &texture2d_desc);
	load_texture_from_file(device, L"./resources/TexMaps/Mask/dissolve_animation.png", textures[1].ReleaseAndGetAddressOf(), &texture2d_desc);
}

void Sphere::play(DirectX::XMFLOAT3 pos)
{
	active = true;
	position = pos;
}

void Sphere::stop()
{
	EffecttBase::stop();
	constants->data.scroll_speed = 0;
}

void Sphere::update(Graphics& graphics, float elapsed_time)
{
}

void Sphere::render(Graphics& graphics)
{
#if USE_IMGUI
	imgui_menu_bar("Effects", "sphere", display_imgui);
	ImGui::Begin("sphere");
	ImGui::DragFloat2("dir", &constants->data.scroll_direction.x, 0.1);
	ImGui::DragFloat("speed", &constants->data.scroll_speed, 0.1);
	ImGui::DragFloat4("particle_color", &constants->data.particle_color.x, 0.1);
	ImGui::DragFloat3("position", &position.x, 0.1);
	ImGui::DragFloat("scale", &scale.x, 0.1);
	ImGui::Checkbox("play", &active);
	ImGui::End();
	scale = { scale.x,scale.x,scale.x };
#endif
	//エフェクトがアクティブ状態になっていたら描画
	if (active)
	{
		//シェーダーをアクティブ状態に
		shader->active(graphics.get_dc().Get(), vertex_shader.Get(), pixel_shader.Get());
		//定数バッファ送信
		constants->bind(graphics.get_dc().Get(), 9, CB_FLAG::PS_VS);
		graphics.get_dc().Get()->PSSetShaderResources(20, 1, textures[0].GetAddressOf());
		graphics.get_dc().Get()->PSSetShaderResources(21, 1, textures[1].GetAddressOf());
		//if(slash)
		DirectX::XMFLOAT4X4 world = Math::calc_world_matrix(scale, orientation, position);
		shader->render(graphics.get_dc().Get(), model.get(), world);
	}
}
