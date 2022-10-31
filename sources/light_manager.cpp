#include "light_manager.h"
#include "user.h"

void LightManager::initialize(Graphics& graphics)
{
	HRESULT hr;
	hr = create_ps_from_cso(graphics.get_device().Get(), "shaders/deferred_light.cso", deferred_light.GetAddressOf());
	hr = create_ps_from_cso(graphics.get_device().Get(), "shaders/deferred_light_shadow.cso", shadow_map_light.GetAddressOf());
	light_screen = std::make_unique<FullscreenQuad>(graphics.get_device().Get());
#if CAST_SHADOW
	DirectX::XMFLOAT3 shadow_light_dir = { 1.0f, 1.0f, -1.0 };
	DirectX::XMFLOAT3 shadow_color = { 0.2f, 0.2f, 0.2f };
	shadow_dir_light = std::make_unique<DirectionalLight>(graphics, shadow_light_dir, shadow_color.x, shadow_color.y, shadow_color.z);
#endif
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void LightManager::register_light(std::string name, std::shared_ptr<Light> light)
{
	int unique_id = 0;
	std::string old_name = name;
	for (auto& light : lights)
	{
		if (name == light.first)
		{
			unique_id++;
			name = old_name + to_string(unique_id);
		}
	}
	lights.insert(make_pair(name, light));
}


void LightManager::draw(Graphics& graphics, ID3D11ShaderResourceView** rtv,int rtv_num)
{
	//‰e—pƒ‰ƒCƒg•`‰æ
	
#if CAST_SHADOW
	shadow_dir_light->light_constants->bind(graphics.get_dc().Get(), 7);
	light_screen->blit(graphics.get_dc().Get(), rtv, 0, rtv_num, shadow_map_light.Get());
#endif
	//’Êíƒ‰ƒCƒg•`‰æ
	for (auto& light : lights)
	{
		light.second->light_constants->bind(graphics.get_dc().Get(), 7);
		light_screen->blit(graphics.get_dc().Get(), rtv, 0, rtv_num, deferred_light.Get());
	}
	
}

void LightManager::debug_gui()
{
	imgui_menu_bar("Lights", "Light", display_imgui);
	if (display_imgui)
	{
#if CAST_SHADOW
		shadow_dir_light->debug_gui(-1);
#endif
		for (auto& light : lights)
		{
			light.second->debug_gui(light.first);
		}
	}
}
