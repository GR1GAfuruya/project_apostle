#pragma once
#include "graphics.h"
#include "framebuffer.h"
#include "fullscreen_quad.h"
class GaussianBlur
{
public:
	/// �������B
	///�K�E�V�A���u���[��������I���W�i���e�N�X�`��
	void Init(ID3D11Device* device, ID3D11ShaderResourceView* originalTexture, uint32_t width,uint32_t height);
	/// <summary>
	/// �K�E�V�A���u���[��GPU��Ŏ��s�B
	void execute_on_GPU(ID3D11DeviceContext* dc, float blurPower, ID3D11ShaderResourceView* originalTexture);
	/// �{�P�e�N�X�`�����擾�B
	ID3D11ShaderResourceView* get_boke_texture()
	{
		return y_blur_frame_buffer->get_color_map().Get();
	}
	struct CB_Gauss
	{
		DirectX::XMFLOAT4 weights[2];
	};
private:
	/// �����_�����O�^�[�Q�b�g���������B
	void init_render_targets(ID3D11Device* device);
	/// �X�v���C�g���������B
	void init_taransfer_image(ID3D11Device* device);
	/// �d�݃e�[�u�����X�V����B
	void update_weights_table(float blurPower);
private:
	enum { NUM_WEIGHTS = 8 };				//�d�݂̐��B
	float weights[NUM_WEIGHTS];			//�d�݃e�[�u���B
	std::unique_ptr<Constants<CB_Gauss>> cb_gauss;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> original_texture = nullptr;	//�I���W�i���e�N�X�`���B
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_x_srv;					//���{�P�摜��`�悷�邽�߂̃X�v���C�g�B
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_y_srv;					//�c�{�P�摜��`�悷�邽�߂̃X�v���C�g�B

	//���u���[
	Microsoft::WRL::ComPtr<ID3D11VertexShader> x_blur_vs;
	std::unique_ptr<FrameBuffer> x_blur_frame_buffer;//���{�P�摜��`�悷��framebuffer�B
	std::unique_ptr<FullscreenQuad> x_blur_taransfer_image;
	//�c�u���[
	Microsoft::WRL::ComPtr<ID3D11VertexShader> y_blur_vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  blur_ps;
	std::unique_ptr<FrameBuffer> y_blur_frame_buffer;//�c�{�P�摜��`�悷��framebuffer�B
	std::unique_ptr<FullscreenQuad> y_blur_taransfer_image;

public:
	uint32_t tex_width_size;
	uint32_t tex_height_size;
};