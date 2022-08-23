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
		float bloom_extraction_threshold;
		float blur_convolution_intensity;
		//RADIAL_BLUR
		DirectX::XMFLOAT2 origin; //���S�_
		float ray_power; //�u���[��������p���[ :�����x�N�g���ɂ����Č����̒��������߂�
		float wipe_threshold;
	
		// �F������
		float hueShift = 1;
		// �ʓx����
		float saturation = 1;
		// ���x����
		float brightness = 1;
		float pad3;
	};

	void begin(ID3D11DeviceContext* dc);
	void end(ID3D11DeviceContext* dc);
	void blit(Graphics& graphics);

	//�K�E�X�u���[
	std::unique_ptr<Bloom> bloom;
	//���摜�ۑ��p�t���[���o�b�t�@�[
	std::unique_ptr<FrameBuffer> original_frame_buffer;
	std::unique_ptr<FrameBuffer> luminance_frame_buffer;
	//std::unique_ptr<FrameBuffer> original;
	std::unique_ptr<FullscreenQuad> luminance;
	std::unique_ptr<FullscreenQuad> final_sprite;

	Microsoft::WRL::ComPtr<ID3D11PixelShader>  luminance_ps;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  post_effects;

	std::unique_ptr<Constants<CB_PostEffect>> cb_post_effect;

	bool display_post_effects_imgui = true;
};