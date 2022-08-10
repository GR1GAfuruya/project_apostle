#include "light_manager.h"

LightManager::LightManager(Graphics& graphics)
{
	HRESULT hr;
	hr = create_ps_from_cso(graphics.get_device().Get(), "shaders/deferred_light.cso", deferred_light.GetAddressOf());
	light_screen = std::make_unique<FullscreenQuad>(graphics.get_device().Get());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void LightManager::register_light(std::shared_ptr<Light> light)
{
	lights.push_back(light);
}


void LightManager::draw(Graphics& graphics, ID3D11ShaderResourceView** rtv,int rtv_num)
{
	for (auto& light : lights)
	{
		light->light_constants->bind(graphics.get_dc().Get(), 7);
		light_screen->blit(graphics.get_dc().Get(), rtv, 0, rtv_num, deferred_light.Get());
	}
	
}

void LightManager::debug_gui()
{
	for (int i = 0; i < lights.size(); i++)
	{
		lights.at(i)->debug_gui(i);
	}
}
