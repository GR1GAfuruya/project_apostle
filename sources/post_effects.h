#pragma once
#include "framebuffer.h"
#include "constant.h"
#include "bloom.h"
#include "graphics.h"
#include <cereal/cereal.hpp>
#define POST_EFFECT PostEffects::POST_EFFECTS
class PostEffects
{
public:
	PostEffects(ID3D11Device* device, const char* post_effect_file_path);
	~PostEffects(){}
	
	//TODO::JSON�����ăv���Z�b�g�̂悤�Ɉ�����悤�ɂ���I�I
	struct CB_PostEffect
	{
		//BLUR
		DirectX::XMFLOAT2 c_sigma = { 1.0f, 0.0f };
		float bloom_extraction_threshold = 1.0f;
		float blur_convolution_intensity = 1.2f;
		//���W�A���u���[
		float radial_power;
		//�V�[���J��
		DirectX::XMFLOAT2 scene_threshold;
		float pad0;
		// �F������
		float hueShift = 1;
		// �ʓx����
		float saturation = 1;
		// ���x����
		float brightness = 2.4f;
		//�R���g���X�g
		float contrast = 7.0f;

		//�r�l�b�g
		float falloff = 0.3f;
		float amount = 0.6f;
		DirectX::XMFLOAT2 pad;
		DirectX::XMFLOAT3 vignette_color;
		float pad2;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				cereal::make_nvp("c_sigma", c_sigma),
				cereal::make_nvp("bloom_extraction_threshold", bloom_extraction_threshold),
				cereal::make_nvp("blur_convolution_intensity", blur_convolution_intensity),
				cereal::make_nvp("radial_power", radial_power),
				cereal::make_nvp("hueShift", hueShift),
				cereal::make_nvp("saturation", saturation),
				cereal::make_nvp("brightness", brightness),
				cereal::make_nvp("contrast", contrast),
				cereal::make_nvp("falloff", falloff),
				cereal::make_nvp("amount", amount),
				cereal::make_nvp("vignette_color", vignette_color)
			);
		}


	};

public:
	void begin(ID3D11DeviceContext* dc);
	void end(ID3D11DeviceContext* dc);
	void blit(Graphics& graphics);
	//----------<�t�@�C��>------------//
	void load_data_file();
	void save_data_file();
	const char* file_path;

	float get_radial_blur_power() { return cb_post_effect.get()->data.radial_power; }
	void set_radial_blur_power(float p) {  cb_post_effect.get()->data.radial_power = p; }
	CB_PostEffect& get_init_param() { return post_effect_init_param; }
	CB_PostEffect& get_now_param();

	void set_posteffect_param(CB_PostEffect param) { cb_post_effect.get()->data = param; };

private:
	//�R���X�^���g�o�b�t�@�̏����p�����[�^�[
	CB_PostEffect post_effect_init_param;
	//�R���X�^���g�o�b�t�@
	std::unique_ptr<Constants<CB_PostEffect>> cb_post_effect;

	//�쐣���u���[��
	std::unique_ptr<Bloom> bloom;
	//���摜�ۑ��p�t���[���o�b�t�@�[
	std::unique_ptr<FrameBuffer> original_frame_buffer;
	std::unique_ptr<FrameBuffer> post_effect_frame_buffer;
	std::unique_ptr<FullscreenQuad> final_sprite;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  dissolve_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  distortion_texture;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  post_effects;


	bool display_post_effects_imgui = false;
};