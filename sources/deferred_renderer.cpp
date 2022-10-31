#include "deferred_renderer.h"
#include "imgui_include.h"
#include "light_manager.h"
#include "texture.h"
#include "user.h"
//==============================================================
// 
// コンストラクタ
// 
//==============================================================
DeferredRenderer::DeferredRenderer(Graphics& graphics)
{
	//G-Buffer
	g_color = std::make_unique<GBuffer>();
	g_depth = std::make_unique<GBuffer>();
	g_normal = std::make_unique<GBuffer>();
	g_position = std::make_unique<GBuffer>();
	g_metal_smooth = std::make_unique<GBuffer>();
	g_emissive = std::make_unique<GBuffer>();
	l_light = std::make_unique<GBuffer>();
	l_composite = std::make_unique<GBuffer>();


	g_color->create(graphics.get_device().Get(), DXGI_FORMAT_R16G16B16A16_FLOAT);
	g_depth->create(graphics.get_device().Get(), DXGI_FORMAT_R32_FLOAT);
	g_normal->create(graphics.get_device().Get(), DXGI_FORMAT_R8G8B8A8_UNORM);
	g_position->create(graphics.get_device().Get(), DXGI_FORMAT_R32G32B32A32_FLOAT);
	g_metal_smooth->create(graphics.get_device().Get(), DXGI_FORMAT_R16G16B16A16_FLOAT);
	g_emissive->create(graphics.get_device().Get(), DXGI_FORMAT_R16G16B16A16_FLOAT);
	l_light->create(graphics.get_device().Get(), DXGI_FORMAT_R16G16B16A16_FLOAT);
	l_composite->create(graphics.get_device().Get(), DXGI_FORMAT_R16G16B16A16_FLOAT);
#if CAST_SHADOW
	//シャドウマップ初期化
	int ShadowMapSize = 1024;
	shadow_frame_buffer = std::make_unique<FrameBuffer>(graphics.get_device().Get(), ShadowMapSize, ShadowMapSize,
		FB_FLAG::COLOR_DEPTH_STENCIL, DXGI_FORMAT_R16G16_FLOAT);
	shadow_constants = std::make_unique<Constants<SHADOW_CONSTANTS>>(graphics.get_device().Get());
#endif
	//深度ステンシル
	depth_stencil_create(graphics.get_device().Get(), DXGI_FORMAT_D24_UNORM_S8_UINT);
	HRESULT hr;

	deferred_screen = std::make_unique<FullscreenQuad>(graphics.get_device().Get());
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	hr = load_texture_from_file(graphics.get_device().Get(), L"./resources/Sprite/emv_map.DDS", env_texture.ReleaseAndGetAddressOf(), &texture2d_desc);

	hr = create_ps_from_cso(graphics.get_device().Get(), "shaders/deferred_env_light.cso", deferred_env_light.GetAddressOf());
	hr = create_ps_from_cso(graphics.get_device().Get(), "shaders/deferred_composite_light.cso", deferred_composite_light.GetAddressOf());
	hr = create_ps_from_cso(graphics.get_device().Get(), "shaders/final_sprite_ps.cso", final_sprite_ps.GetAddressOf());
	if (FAILED(hr)) return;
}


//==============================================================
// 
// ディファードレンダー書き込み開始
// 
//==============================================================
void DeferredRenderer::active(Graphics& graphics)
{
	//RTVを変更する前に使用中のRTVを保存
	graphics.get_dc()->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(), cached_depth_stencil_view.ReleaseAndGetAddressOf());

	ID3D11RenderTargetView* targets[] = {
		g_color->get_rtv(),   //Target0
		g_depth->get_rtv(),   //Target1
		g_normal->get_rtv(),   //Target2
		g_position->get_rtv(),   //Target3
		g_metal_smooth->get_rtv(),  //Target4
		g_emissive->get_rtv(),  //Target5

	};
	// レンダーターゲットビュー設定
	graphics.get_dc()->OMSetRenderTargets(
		6, targets, depth_stencil_view.Get());
	//レンダーターゲットビューのクリア
	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	graphics.get_dc()->ClearRenderTargetView(g_color->get_rtv(), clearColor);

	float clear_metallic_smooth[4] = { 0.8f, 0.1f, 0, 0 };
	graphics.get_dc()->ClearRenderTargetView(g_metal_smooth->get_rtv(), clear_metallic_smooth);

	float clear_pos_normal_light[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	graphics.get_dc()->ClearRenderTargetView(g_normal->get_rtv(), clear_pos_normal_light);
	graphics.get_dc()->ClearRenderTargetView(g_position->get_rtv(), clear_pos_normal_light);
	graphics.get_dc()->ClearRenderTargetView(g_emissive->get_rtv(), clear_pos_normal_light);

	float cleardepth[4] = { 5000, 1.0f, 1.0f, 1.0f };
	graphics.get_dc()->ClearRenderTargetView(g_depth->get_rtv(), cleardepth);

	//深度ステンシルビューのクリア
	graphics.get_dc()->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ビューポート設定
	D3D11_VIEWPORT vp = {
		0,0,
		static_cast<float>(SCREEN_WIDTH),
		static_cast<float>(SCREEN_HEIGHT),
		0,1 };
	graphics.get_dc()->RSSetViewports(1, &vp);
}

//==============================================================
// 
// ディファードレンダー書き込み終了
// 
//==============================================================
void DeferredRenderer::deactive(Graphics& graphics)
{
	//ライティング実行
	lighting(graphics);
	//描画先を戻す
	graphics.get_dc()->OMSetRenderTargets(1, cached_render_target_view.GetAddressOf(),
		cached_depth_stencil_view.Get());
	
}
//==============================================================
// 
// ライティング
// 
//==============================================================
void DeferredRenderer::lighting(Graphics& graphics) const
{
	ID3D11RenderTargetView* rtv = l_light->get_rtv();
	// レンダーターゲットビュー設定
	graphics.get_dc()->OMSetRenderTargets(
		1, &rtv, depth_stencil_view.Get());
	//レンダーターゲットビューのクリア
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	graphics.get_dc()->ClearRenderTargetView(l_light->get_rtv(), clearColor);

	//G-Buffer：カラー、ノーマル、ポジション、メタリック・スムース、ライト
	ID3D11ShaderResourceView* g_buffers[]
	{
		g_color->get_srv(),
		g_normal->get_srv(),
		g_position->get_srv(),
		g_metal_smooth->get_srv(),
		g_emissive->get_srv(),
		l_light->get_srv(),
	};
	//ブレンドステートを加算に
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_OFF_ZW_OFF, ST_BLEND::ADD, ST_RASTERIZER::CULL_NONE);

	UINT G_BUFFERS_NUM = ARRAYSIZE(g_buffers);
	//環境ライト
	graphics.get_dc().Get()->PSSetShaderResources(15, 1, env_texture.GetAddressOf());
	deferred_screen->blit(graphics.get_dc().Get(), g_buffers, 0, G_BUFFERS_NUM, deferred_env_light.Get());

	//平行光、点光源ライトを描き込む
#if CAST_SHADOW
	graphics.get_dc().Get()->PSSetShaderResources(16, 1, shadow_frame_buffer->get_color_map().GetAddressOf());
#endif
	LightManager::instance().draw(graphics, g_buffers, G_BUFFERS_NUM);

	//ライトの合成 ブレンドステートをアルファに
	graphics.set_blend_state(ST_BLEND::ALPHA);
	rtv = l_composite->get_rtv();
	graphics.get_dc()->OMSetRenderTargets(
		1, &rtv, depth_stencil_view.Get());
	graphics.get_dc()->ClearRenderTargetView(l_composite->get_rtv(), clearColor);

	deferred_screen->blit(graphics.get_dc().Get(), g_buffers, 0, G_BUFFERS_NUM, deferred_composite_light.Get());

	LightManager::instance().debug_gui();
}

//==============================================================
// 
// 描画
// 
//==============================================================
void DeferredRenderer::render(Graphics& graphics)
{
	ID3D11ShaderResourceView* g_buffers[]
	{
		l_composite->get_srv(),
	};
	UINT G_BUFFERS_NUM = ARRAYSIZE(g_buffers);
	deferred_screen->blit(graphics.get_dc().Get(), g_buffers, 0, G_BUFFERS_NUM, final_sprite_ps.Get());

#if USE_IMGUI
	imgui_menu_bar("Window", "G-Buffer", display_imgui);
	if (display_imgui)
	{
		ImGui::Begin("G -Buffer");
		ImGui::Text("color");
		ImGui::Image(g_color->get_srv(), { 1280 * (ImGui::GetWindowSize().x / 1280),  720 * (ImGui::GetWindowSize().y / 720) });
		ImGui::Text("depth");
		ImGui::Image(g_depth->get_srv(), { 1280 * (ImGui::GetWindowSize().x / 1280),  720 * (ImGui::GetWindowSize().y / 720) });
		ImGui::Text("normal");
		ImGui::Image(g_normal->get_srv(), { 1280 * (ImGui::GetWindowSize().x / 1280),  720 * (ImGui::GetWindowSize().y / 720) });
		ImGui::Text("position");
		ImGui::Image(g_position->get_srv(), { 1280 * (ImGui::GetWindowSize().x / 1280),  720 * (ImGui::GetWindowSize().y / 720) });
		ImGui::Text("metal_smooth");
		ImGui::Image(g_metal_smooth->get_srv(), { 1280 * (ImGui::GetWindowSize().x / 1280),  720 * (ImGui::GetWindowSize().y / 720) });
		ImGui::Text("light");
		ImGui::Image(l_light->get_srv(), { 1280 * (ImGui::GetWindowSize().x / 1280),  720 * (ImGui::GetWindowSize().y / 720) });
#if CAST_SHADOW
		ImGui::Text("shadow");
		ImGui::Image(shadow_frame_buffer->get_color_map().Get(), { 1280 * (ImGui::GetWindowSize().x / 1280),  720 * (ImGui::GetWindowSize().y / 720) });
#endif

		ImGui::End();

	}
#endif // USE_IMGUI
}
//==============================================================
// 
// 影用の描画書き込み開始
// 
//==============================================================
#if CAST_SHADOW
void DeferredRenderer::shadow_active(Graphics& graphics)
{
	shadow_frame_buffer->clear(graphics.get_dc().Get());
	shadow_frame_buffer->activate(graphics.get_dc().Get());

	// 平行光源からカメラ位置を作成し、そこから原点の位置を見るように視線行列を生成
	DirectX::XMVECTOR TargetPosition = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 shadow_dir_light_dir = light_manager.get_shadow_dir_light_dir();
	DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&shadow_dir_light_dir);
	LightPosition = DirectX::XMVectorScale(LightPosition, -50.0f);
	LightPosition = DirectX::XMVectorAdd(LightPosition, TargetPosition);
	DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(LightPosition,
		TargetPosition,
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(30, 30, 10, 100);
	DirectX::XMMATRIX VP = V * P;
	DirectX::XMFLOAT4X4	shadowVP;
	DirectX::XMStoreFloat4x4(&shadow_constants->data.shadowVP, VP);
	shadow_constants->bind(graphics.get_dc().Get(), 10, CB_FLAG::PS_VS);

}
//==============================================================
// 
// 影用の描画書き込み終了
// 
//==============================================================
void DeferredRenderer::shadow_deactive(Graphics& graphics)
{
	shadow_frame_buffer->deactivate(graphics.get_dc().Get());
}
#endif
//==============================================================
// 
// 深度バッファ生成
// 
//==============================================================
void DeferredRenderer::depth_stencil_create(ID3D11Device* device, DXGI_FORMAT format)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer;
	// 深度ステンシル設定
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.Width = SCREEN_WIDTH;
	td.Height = SCREEN_HEIGHT;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = format;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL ;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	// 深度ステンシルテクスチャ生成
	hr = device->CreateTexture2D(&td, NULL, depth_stencil_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = td.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;

	// 深度ステンシルビュー生成
	hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &dsvd, depth_stencil_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	
}
