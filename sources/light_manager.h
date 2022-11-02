#pragma once
#define CAST_SHADOW 0
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
	void initialize(Graphics& graphics);
	//���C�g���Ď��Ώۂɒǉ�
	void  register_light(std::string name, std::shared_ptr<Light> light);
	//���C�g��G-Buffer�ɑ���
	void draw(Graphics& graphics, ID3D11ShaderResourceView** rtv, int rtv_num);

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
	std::unique_ptr<DirectionalLight> shadow_dir_light;//�V���h�E�}�b�v�p���C�g
#endif
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shadow_map_light;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_light;
	
	bool display_imgui = false;

};