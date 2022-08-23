#pragma once
#include "graphics.h"
#include "g_buffer.h"
#include "fullscreen_quad.h"
#include "light_manager.h"
class DeferredRenderer
{
public:
	DeferredRenderer(Graphics& graphics);
	~DeferredRenderer() {}


	void active(Graphics& graphics);
	void deactive(Graphics& graphics, LightManager& light_manager);
	void render(Graphics& graphics);

	void lighting(Graphics& graphics,LightManager& light_manager) const;

	ID3D11DepthStencilView* get_dsv() { return depth_stencil_view.Get(); }
private:
	void depth_stencil_create(ID3D11Device* device, DXGI_FORMAT format);

	std::unique_ptr<GBuffer> g_color;
	std::unique_ptr<GBuffer> g_depth;
	std::unique_ptr<GBuffer> g_normal;
	std::unique_ptr<GBuffer> g_position;
	std::unique_ptr<GBuffer> g_metal_smooth;
	std::unique_ptr<FullscreenQuad> deferred_screen;
	

	std::unique_ptr<GBuffer> l_light;
	std::unique_ptr<GBuffer> l_composite;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> final_sprite_ps;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_env_light;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_composite_light;
	//�[�x�X�e���V���r���[
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cached_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cached_depth_stencil_view;

};