#include "light_manager.h"

LightManager::LightManager(Graphics& graphics)
{
	HRESULT hr;
	hr = create_ps_from_cso(graphics.get_device().Get(), "shaders/deferred_light.cso", deferred_light.GetAddressOf());
	hr = create_ps_from_cso(graphics.get_device().Get(), "shaders/deferred_light_shadow.cso", shadow_map_light.GetAddressOf());
	light_screen = std::make_unique<FullscreenQuad>(graphics.get_device().Get());
	DirectX::XMFLOAT3 shadow_light_dir = { 1.0f, 1.0f, -1.0 };
	DirectX::XMFLOAT3 shadow_color = { 0.2f, 0.2f, 0.2f };
	shadow_dir_light = std::make_unique<DirectionalLight>(graphics, shadow_light_dir, shadow_color.x, shadow_color.y, shadow_color.z);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void LightManager::register_light(std::shared_ptr<Light> light)
{
	lights.push_back(light);
}


void LightManager::draw(Graphics& graphics, ID3D11ShaderResourceView** rtv,int rtv_num)
{
	//‰e—pƒ‰ƒCƒg•`‰æ
	shadow_dir_light->light_constants->bind(graphics.get_dc().Get(), 7);
	light_screen->blit(graphics.get_dc().Get(), rtv, 0, rtv_num, shadow_map_light.Get());
	//’Êíƒ‰ƒCƒg•`‰æ
	for (auto& light : lights)
	{
		light->light_constants->bind(graphics.get_dc().Get(), 7);
		light_screen->blit(graphics.get_dc().Get(), rtv, 0, rtv_num, deferred_light.Get());
	}
	
}

void LightManager::debug_gui()
{
	shadow_dir_light->debug_gui(-1);
	for (int i = 0; i < lights.size(); i++)
	{
		lights.at(i)->debug_gui(i);
	}
}
