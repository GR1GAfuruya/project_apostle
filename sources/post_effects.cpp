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
	post_effect_frame_buffer = make_unique<FrameBuffer>(device,
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
	imgui_menu_bar("window", "post_effect", display_post_effects_imgui);
	if (display_post_effects_imgui)
	{
	ImGui::Begin("PostEffect");
	if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("original");
		ImGui::Image(original_frame_buffer->get_color_map().Get(), { SCREEN_WIDTH * (ImGui::GetWindowSize().x / SCREEN_WIDTH),  SCREEN_HEIGHT * (ImGui::GetWindowSize().y / SCREEN_HEIGHT) });
		ImGui::Text("lumina");
		ImGui::Image(post_effect_frame_buffer->get_color_map().Get(), { SCREEN_WIDTH * (ImGui::GetWindowSize().x / SCREEN_WIDTH),  SCREEN_HEIGHT * (ImGui::GetWindowSize().y / SCREEN_HEIGHT) });
	}
	//パラメータ設定
	if (display_post_effects_imgui)
	{
		if (ImGui::CollapsingHeader("Param", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat("hueShift", &cb_post_effect->data.hueShift, 0.1f, 0.0f, 10.0f);
			ImGui::DragFloat("saturation", &cb_post_effect->data.saturation, 0.1f, 0.0f, 10.0f);
			ImGui::DragFloat("brightness", &cb_post_effect->data.brightness, 0.1f, 0.0f, 10.0f);
		}
	}
	ImGui::End();
	}
#endif
}


void PostEffects::blit(Graphics& graphics)
{
	
	//定数バッファ設定
	cb_post_effect->bind(graphics.get_dc().Get(), 5);
	//レンダーターゲットを戻す
	post_effect_frame_buffer->clear(graphics.get_dc().Get(), FB_FLAG::COLOR);
	post_effect_frame_buffer->activate(graphics.get_dc().Get(), FB_FLAG::COLOR);
	final_sprite->blit(graphics.get_dc().Get(), original_frame_buffer->get_color_map().GetAddressOf(), 0, 1);

	bloom->make(graphics.get_dc().Get(), original_frame_buffer->get_color_map().Get());
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_OFF_ZW_OFF, ST_BLEND::ADD, ST_RASTERIZER::CULL_NONE);
	bloom->blit(graphics.get_dc().Get());
	post_effect_frame_buffer->deactivate(graphics.get_dc().Get());
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_OFF_ZW_OFF, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	final_sprite->blit(graphics.get_dc().Get(), post_effect_frame_buffer->get_color_map().GetAddressOf(), 0, 1, post_effects.Get());

#if USE_IMGUI
	if (display_post_effects_imgui)
	{
		ImGui::Begin("PostEffect");
		ImGui::DragFloat("blur_threshold", &cb_post_effect->data.bloom_extraction_threshold, 0.1f, -1.0f, 10.0f);
		ImGui::DragFloat("blur_intensity", &cb_post_effect->data.blur_convolution_intensity, 0.1f, 0, 20);
		ImGui::End();
		graphics.recompile_pixel_shader(post_effects.GetAddressOf(),"_post");
	}
#endif

}
