#include "aura.h"


#include "imgui_include.h"
#include "device.h"
#include "texture.h"

Aura::Aura(Graphics& graphics)
{
	create_vs_from_cso(graphics.get_device().Get(), "shaders\\aura_vs.cso",
		vertex_shader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	create_ps_from_cso(graphics.get_device().Get(), "shaders\\fire_ps.cso", pixel_shader.ReleaseAndGetAddressOf());
    aura_constants = std::make_unique<Constants<AURA_CONSTANTS>>(graphics.get_device().Get());
    D3D11_TEXTURE2D_DESC texture2d_desc{};
	load_texture_from_file(graphics.get_device().Get(), L".\\resources\\TexMaps\\Mask\\dissolve_animation.png", shader_resource_views.ReleaseAndGetAddressOf(), &texture2d_desc);

}


void Aura::update(Graphics& graphics,float elapsed_time)
{
	Mouse& mouse = Device::instance().get_mouse();
	compile_shader(graphics.get_device().Get(), L"HLSL\\fire_ps.cso", pixel_shader.Get());
    aura_constants->bind(graphics.get_dc().Get(), 6);
	graphics.get_dc().Get()->PSSetShaderResources(5, 1, shader_resource_views.GetAddressOf());

   
}
