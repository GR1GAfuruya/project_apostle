#include "gaussian_blur.h"
#include "imgui_include.h"
void GaussianBlur::Init(ID3D11Device* device, ID3D11ShaderResourceView* originalTexture, uint32_t width, uint32_t height)
{
	original_texture = originalTexture;
	tex_width_size = width;
	tex_height_size = height;
	
	create_vs_from_cso(device, "shaders/gaussian_blurX_vs.cso", x_blur_vs.ReleaseAndGetAddressOf(),
		nullptr, nullptr, 0);
	create_vs_from_cso(device, "shaders/gaussian_blurY_vs.cso", y_blur_vs.ReleaseAndGetAddressOf(),
		nullptr, nullptr, 0);
	create_ps_from_cso(device, "shaders/gaussian_blur_ps.cso", blur_ps.ReleaseAndGetAddressOf());

	//レンダリングターゲットを初期化。
	init_render_targets(device);
	//スプライトを初期化。
	init_taransfer_image(device);

	//定数バッファ
	cb_gauss = std::make_unique<Constants<CB_Gauss>>(device);
}

void GaussianBlur::execute_on_GPU(ID3D11DeviceContext* dc, float blurPower, ID3D11ShaderResourceView* originalTexture)
{
	original_texture = originalTexture;
	//重みテーブルを更新する。
	update_weights_table(blurPower);
	//定数バッファ設定
	cb_gauss->bind(dc, 0);
	//横ブラーを実行	
	//レンダリングターゲットをクリア。
	x_blur_frame_buffer->clear(dc, FB_FLAG::COLOR,{ 0, 0, 0, 1 });
	x_blur_frame_buffer->activate(dc, FB_FLAG::COLOR);
	x_blur_taransfer_image->blit(dc, original_texture.GetAddressOf(),
		0, 1, blur_ps.Get(), x_blur_vs.Get());
	x_blur_frame_buffer->deactivate(dc);


	y_blur_frame_buffer->clear(dc, FB_FLAG::COLOR);
	y_blur_frame_buffer->activate(dc, FB_FLAG::COLOR);
	y_blur_taransfer_image->blit(dc, x_blur_frame_buffer->get_color_map().GetAddressOf(),
		0, 1, blur_ps.Get(), y_blur_vs.Get());
	y_blur_frame_buffer->deactivate(dc);
}

void GaussianBlur::init_render_targets(ID3D11Device* device)
{
	//レンダリングターゲット初期化
	tex_width_size = tex_width_size / 2;
	x_blur_frame_buffer = make_unique<FrameBuffer>(device,
		tex_width_size, tex_height_size, FB_FLAG::COLOR);

	tex_height_size = tex_height_size / 2;
	y_blur_frame_buffer = make_unique<FrameBuffer>(device,
		tex_width_size, tex_height_size, FB_FLAG::COLOR);
}

void GaussianBlur::init_taransfer_image(ID3D11Device* device)
{
	x_blur_taransfer_image = make_unique<FullscreenQuad>(device);
	y_blur_taransfer_image = make_unique<FullscreenQuad>(device);
}

void GaussianBlur::update_weights_table(float blurPower)
{
	float total = 0;
	for (int i = 0; i < NUM_WEIGHTS; i++) {
		weights[i] = expf(-0.5f * (float)(i * i) / blurPower);
		total += 2.0f * weights[i];
	}
	// 規格化
	for (int i = 0; i < NUM_WEIGHTS; i++) {
		weights[i] /= total;
	}

	cb_gauss->data.weights[0] = { weights[0],weights[1], weights[2], weights[3] };
	cb_gauss->data.weights[1] = { weights[4],weights[5], weights[6], weights[7] };
}
