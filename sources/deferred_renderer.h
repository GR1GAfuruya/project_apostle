#pragma once
#include "graphics.h"
#include "g_buffer.h"
#include "fullscreen_quad.h"
#include "framebuffer.h"
#include "light_manager.h"
class DeferredRenderer
{
public:
	DeferredRenderer(Graphics& graphics);
	~DeferredRenderer() {}


	void active(Graphics& graphics);
	void deactive(Graphics& graphics);
	void render(Graphics& graphics);
#if CAST_SHADOW
	void shadow_active(Graphics& graphics);
	void shadow_deactive(Graphics& graphics);
#endif
	void lighting(Graphics& graphics) const;

	ID3D11DepthStencilView* get_dsv() { return depth_stencil_view.Get(); }
private:
	void depth_stencil_create(ID3D11Device* device, DXGI_FORMAT format);

	std::unique_ptr<GBuffer> g_color;
	std::unique_ptr<GBuffer> g_depth;
	std::unique_ptr<GBuffer> g_normal;
	std::unique_ptr<GBuffer> g_position;
	std::unique_ptr<GBuffer> g_metal_smooth;
	std::unique_ptr<GBuffer> g_emissive;
	std::unique_ptr<FullscreenQuad> deferred_screen;
	

	std::unique_ptr<GBuffer> l_light;
	std::unique_ptr<GBuffer> l_composite;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> final_sprite_ps;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_env_light;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_composite_light;

	//シャドウマップ
#if CAST_SHADOW
	struct SHADOW_CONSTANTS
	{
		DirectX::XMFLOAT4X4	shadowVP;
	};
	std::unique_ptr<Constants<SHADOW_CONSTANTS>> shadow_constants{};
	std::unique_ptr<FrameBuffer> shadow_frame_buffer;
#endif
	//深度ステンシルビュー
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cached_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cached_depth_stencil_view;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> env_texture;

	bool display_imgui = false;

};
