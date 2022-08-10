#pragma once
#include "graphics.h"
#include "framebuffer.h"
#include "fullscreen_quad.h"
class GaussianBlur
{
public:
	/// 初期化。
	///ガウシアンブラーをかけるオリジナルテクスチャ
	void Init(ID3D11Device* device, ID3D11ShaderResourceView* originalTexture, uint32_t width,uint32_t height);
	/// <summary>
	/// ガウシアンブラーをGPU上で実行。
	void execute_on_GPU(ID3D11DeviceContext* dc, float blurPower, ID3D11ShaderResourceView* originalTexture);
	/// ボケテクスチャを取得。
	ID3D11ShaderResourceView* get_boke_texture()
	{
		return y_blur_frame_buffer->get_color_map().Get();
	}
	struct CB_Gauss
	{
		DirectX::XMFLOAT4 weights[2];
	};
private:
	/// レンダリングターゲットを初期化。
	void init_render_targets(ID3D11Device* device);
	/// スプライトを初期化。
	void init_taransfer_image(ID3D11Device* device);
	/// 重みテーブルを更新する。
	void update_weights_table(float blurPower);
private:
	enum { NUM_WEIGHTS = 8 };				//重みの数。
	float weights[NUM_WEIGHTS];			//重みテーブル。
	std::unique_ptr<Constants<CB_Gauss>> cb_gauss;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> original_texture = nullptr;	//オリジナルテクスチャ。
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_x_srv;					//横ボケ画像を描画するためのスプライト。
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_y_srv;					//縦ボケ画像を描画するためのスプライト。

	//横ブラー
	Microsoft::WRL::ComPtr<ID3D11VertexShader> x_blur_vs;
	std::unique_ptr<FrameBuffer> x_blur_frame_buffer;//横ボケ画像を描画するframebuffer。
	std::unique_ptr<FullscreenQuad> x_blur_taransfer_image;
	//縦ブラー
	Microsoft::WRL::ComPtr<ID3D11VertexShader> y_blur_vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  blur_ps;
	std::unique_ptr<FrameBuffer> y_blur_frame_buffer;//縦ボケ画像を描画するframebuffer。
	std::unique_ptr<FullscreenQuad> y_blur_taransfer_image;

public:
	uint32_t tex_width_size;
	uint32_t tex_height_size;
};