#pragma once
#define CAST_SHADOW 1
#include "graphics.h"
#include "light.h"
#include "fullscreen_quad.h"
#include <map>
class LightManager
{
private:
	LightManager() {};
	~LightManager();
public:
	void initialize();
	//ライトを監視対象に追加
	void  register_light(std::string name, std::shared_ptr<Light> light);
	//ライトをG-Bufferに送る
	void draw(ID3D11ShaderResourceView** rtv, int rtv_num);

	void debug_gui();

	void delete_light(std::string name);

	static LightManager& instance()
	{
		static LightManager light_manager;
		return light_manager;
	}

#if CAST_SHADOW
	DirectX::XMFLOAT3 get_shadow_dir_light_dir() { return shadow_dir_light.get()->get_direction(); }
#endif
private:
	std::map<std::string, std::weak_ptr<Light>>lights;
	std::unique_ptr<FullscreenQuad> light_screen;
#if CAST_SHADOW
	std::shared_ptr<DirectionalLight> shadow_dir_light;//シャドウマップ用ライト
#endif
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shadow_map_light;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_light;

	bool display_imgui = false;

};