#pragma once
#include "framebuffer.h"
#include "constant.h"
#include "bloom.h"
#include "graphics.h"
#define POST_EFFECT PostEffects::POST_EFFECTS
class PostEffects
{
public:
	PostEffects(ID3D11Device* device);
	
	~PostEffects(){}
public:
	struct CB_PostEffect
	{
		//LUMINA
		DirectX::XMFLOAT4 l_min = { 0.6f, 0.0f, 1.0f, 0.0f };
		DirectX::XMFLOAT4 l_max = { 0.8f, 0.0f, 1.0f, 0.0f };
		DirectX::XMFLOAT4 lumina = { 0.299f, 0.587f, 0.114f, 0.0f };
		//BLUR
		DirectX::XMFLOAT2 c_sigma = { 1.0f, 0.0f };
		float bloom_extraction_threshold = 1.0f;
		float blur_convolution_intensity = 1.2f;
		//RADIAL_BLUR
		DirectX::XMFLOAT2 origin; //中心点
		float ray_power; //ブラーをかけるパワー :方向ベクトルにかけて光線の長さを決める
		float wipe_threshold;
	
		// 色相調整
		float hueShift = 1;
		// 彩度調整
		float saturation = 1;
		// 明度調整
		float brightness = 2.4f;
		//コントラスト
		float contrast = 7.0f;

		//ビネット
		float falloff = 0.3f;
		float amount = 0.6f;
		DirectX::XMFLOAT2 pad;
	};

	void begin(ID3D11DeviceContext* dc);
	void end(ID3D11DeviceContext* dc);
	void blit(Graphics& graphics);

	//ガウスブラー
	std::unique_ptr<Bloom> bloom;
	//元画像保存用フレームバッファー
	std::unique_ptr<FrameBuffer> original_frame_buffer;
	std::unique_ptr<FrameBuffer> post_effect_frame_buffer;
	std::unique_ptr<FullscreenQuad> final_sprite;

	Microsoft::WRL::ComPtr<ID3D11PixelShader>  post_effects;

	std::unique_ptr<Constants<CB_PostEffect>> cb_post_effect;

	bool display_post_effects_imgui = false;
};