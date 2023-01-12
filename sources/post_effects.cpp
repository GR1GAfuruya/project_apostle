#include "post_effects.h"

#include "framework.h"
#include "shader.h"
#include "user.h"


PostEffects::PostEffects(ID3D11Device* device)
{
	//frame�o�b�t�@�[������
	//���摜
	original_frame_buffer = make_unique<FrameBuffer>(device,
		SCREEN_WIDTH, SCREEN_HEIGHT);
	//�P�x���o�摜
	post_effect_frame_buffer = make_unique<FrameBuffer>(device,
		SCREEN_WIDTH, SCREEN_HEIGHT,FB_FLAG::COLOR);

	//�ŏI�o��
	final_sprite = make_unique<FullscreenQuad>(device);

	//-----�쐣���u���[��-----//
	bloom = make_unique<Bloom>(device,SCREEN_WIDTH,SCREEN_HEIGHT);

	//�萔�o�b�t�@
	cb_post_effect = std::make_unique<Constants<CB_PostEffect>>(device);

	//�V�F�[�_�[
	create_ps_from_cso(device, "shaders/post_effects.cso", post_effects.ReleaseAndGetAddressOf());
}

//�|�X�g�G�t�F�N�g�p��RTV�ɏ������݊J�n
void PostEffects::begin(ID3D11DeviceContext* dc)
{
	original_frame_buffer->clear(dc);
	original_frame_buffer->activate(dc);
}
//�`�����ݐ��RTV��߂�
void PostEffects::end(ID3D11DeviceContext* dc)
{
	original_frame_buffer->deactivate(dc);
}


void PostEffects::blit(Graphics& graphics)
{
	
	//�萔�o�b�t�@�ݒ�
	cb_post_effect->bind(graphics.get_dc().Get(), 5);
	//�����_�[�^�[�Q�b�g��߂�
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
	imgui_menu_bar("Window", "post_effect", display_post_effects_imgui);
	if (display_post_effects_imgui)
	{
		ImGui::Begin("PostEffect");
		//�p�����[�^�ݒ�
		if (display_post_effects_imgui)
		{
			if (ImGui::CollapsingHeader("Param", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::DragFloat2("origin", &cb_post_effect->data.origin.x, 1.0f, 0.0f, 20.0f);
				ImGui::DragFloat("hueShift", &cb_post_effect->data.hueShift, 0.1f, 0.0f, 10.0f);
				ImGui::DragFloat("hueShift", &cb_post_effect->data.hueShift, 0.1f, 0.0f, 10.0f);
				ImGui::DragFloat("saturation", &cb_post_effect->data.saturation, 0.1f, 0.0f, 10.0f);
				ImGui::DragFloat("brightness", &cb_post_effect->data.brightness, 0.1f, 1.0f, 10.0f);
				ImGui::DragFloat("contrast", &cb_post_effect->data.contrast, 0.1f, 0.0f, 50.0f);
				ImGui::DragFloat("ray_power", &cb_post_effect->data.ray_power, 0.1f, 0.0f, 50.0f);
				ImGui::DragFloat("blur_threshold", &cb_post_effect->data.bloom_extraction_threshold, 0.1f, -1.0f, 10.0f);
				ImGui::DragFloat("blur_intensity", &cb_post_effect->data.blur_convolution_intensity, 0.1f, 0, 20);
				ImGui::DragFloat("falloff", &cb_post_effect->data.falloff, 0.1f, 0, 1.0f);
				ImGui::DragFloat("amount", &cb_post_effect->data.amount, 0.1f, 0, 1.0f);
				ImGui::DragFloat("amount", &cb_post_effect->data.radial_power, 0.1f, 0, 1.0f);

			}
			if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("original");
				ImGui::Image(original_frame_buffer->get_color_map().Get(), { SCREEN_WIDTH * (ImGui::GetWindowSize().x / SCREEN_WIDTH),  SCREEN_HEIGHT * (ImGui::GetWindowSize().y / SCREEN_HEIGHT) });
			}

		}
		ImGui::End();
	}
#endif

}
