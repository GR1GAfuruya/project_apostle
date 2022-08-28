#include "post_effects.h"

#include "framework.h"
#include "shader.h"
#include "user.h"


PostEffects::PostEffects(ID3D11Device* device)
{
	//frameバッファー初期化
	//元画像
	original_frame_buffer = make_unique<FrameBuffer>(device,
		SCREEN_WIDTH, SCREEN_HEIGHT);
	//輝度抽出画像
	luminance_frame_buffer = make_unique<FrameBuffer>(device,
		SCREEN_WIDTH, SCREEN_HEIGHT,FB_FLAG::COLOR);

	//輝度抽出
	luminance = make_unique<FullscreenQuad>(device);
	//最終出力
	final_sprite = make_unique<FullscreenQuad>(device);

	//-----川瀬式ブルーム-----//
	bloom = make_unique<Bloom>(device,SCREEN_WIDTH,SCREEN_HEIGHT);

	//定数バッファ
	cb_post_effect = std::make_unique<Constants<CB_PostEffect>>(device);

	//シェーダー
	create_ps_from_cso(device, "shaders/luminance_extraction_ps.cso", luminance_ps.ReleaseAndGetAddressOf());
	create_ps_from_cso(device, "shaders/post_effects.cso", post_effects.ReleaseAndGetAddressOf());
}

//ポストエフェクト用のRTVに書き込み開始
void PostEffects::begin(ID3D11DeviceContext* dc)
{
	original_frame_buffer->clear(dc);
	original_frame_buffer->activate(dc);
}
//描き込み先のRTVを戻す
void PostEffects::end(ID3D11DeviceContext* dc)
{
	original_frame_buffer->deactivate(dc);
#if USE_IMGUI
	ImGui::Begin("PostEffectImage");
	ImGui::Text("original");
	ImGui::Image(original_frame_buffer->get_color_map().Get(), { 1280 * (ImGui::GetWindowSize().x / 1280),  720 * (ImGui::GetWindowSize().y / 720) });
	ImGui::End();
#endif
	
	//輝度抽出
	/*luminance_frame_buffer->clear(dc, FB_FLAG::COLOR, { 0, 0, 0, 1 });
	luminance_frame_buffer->activate(dc, FB_FLAG::COLOR);
	luminance->blit(dc, original_frame_buffer->get_color_map().GetAddressOf(),
		0, 1, luminance_ps.Get());
	luminance_frame_buffer->deactivate(dc);*/

#if USE_IMGUI
	ImGui::Begin("PostEffectImage");
	ImGui::Text("lumina");
	ImGui::Image(luminance_frame_buffer->get_color_map().Get(), { 1280 * (ImGui::GetWindowSize().x / 1280),  720 * (ImGui::GetWindowSize().y / 720) });
	ImGui::End();
	//パラメータ設定
	ImGui::Begin("PostEffect");
	imgui_menu_bar("objects", "post_effect", display_post_effects_imgui);
	if (display_post_effects_imgui)
	{
		if (ImGui::CollapsingHeader("Param", ImGuiTreeNodeFlags_DefaultOpen))
		{
			/*ImGui::DragFloat("l_min", &cb_post_effect->data.l_min.x, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("l_max", &cb_post_effect->data.l_max.x, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat4("lumina", &cb_post_effect->data.lumina.x, 0.1f, 0.0f, 1.0f);*/
			ImGui::DragFloat("hueShift", &cb_post_effect->data.hueShift, 0.1f, 0.0f, 10.0f);
			ImGui::DragFloat("saturation", &cb_post_effect->data.saturation, 0.1f, 0.0f, 10.0f);
			ImGui::DragFloat("brightness", &cb_post_effect->data.brightness, 0.1f, 0.0f, 10.0f);

		}
	}
	ImGui::End();
#endif
}


void PostEffects::blit(Graphics& graphics)
{
	
	//定数バッファ設定
	cb_post_effect->bind(graphics.get_dc().Get(), 5);
	//レンダーターゲットを戻す
	final_sprite->blit(graphics.get_dc().Get(), original_frame_buffer->get_color_map().GetAddressOf(), 0, 1,post_effects.Get());

	bloom->make(graphics.get_dc().Get(), original_frame_buffer->get_color_map().Get());
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_OFF_ZW_OFF, ST_BLEND::ADD, ST_RASTERIZER::CULL_NONE);
	bloom->blit(graphics.get_dc().Get());
#if USE_IMGUI
	if (display_post_effects_imgui)
	{
		ImGui::Begin("PostEffect");
		ImGui::DragFloat("blur_threshold", &cb_post_effect->data.bloom_extraction_threshold, 0.1f, -1.0f, 10.0f);
		ImGui::DragFloat("blur_intensity", &cb_post_effect->data.blur_convolution_intensity, 0.1f, 0, 20);
		ImGui::End();
	}
#endif

}
