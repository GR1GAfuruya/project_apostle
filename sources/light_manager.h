#pragma once
#include "graphics.h"
#include "light.h"
#include "fullscreen_quad.h"
class LightManager
{
public:
	LightManager(Graphics& graphics);
	~LightManager(){}

	//���C�g�̒ǉ�
	void register_light(std::shared_ptr<Light> light);
	//���C�g��G-Buffer�ɑ���
	void draw(Graphics& graphics, ID3D11ShaderResourceView** rtv, int rtv_num);

	void debug_gui();

	DirectX::XMFLOAT3 get_shadow_dir_light_dir() { return shadow_dir_light.get()->get_direction(); }
private:
	std::vector<std::shared_ptr<Light>> lights;
	std::unique_ptr<FullscreenQuad> light_screen;
	std::unique_ptr<DirectionalLight> shadow_dir_light;//�V���h�E�}�b�v�p���C�g

	Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_light;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shadow_map_light;
	


};