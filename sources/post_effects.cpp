#include "post_effects.h"

#include "framework.h"
#include "shader.h"
#include "user.h"
#include "texture.h"

#include <filesystem>
#include <fstream>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>


PostEffects::PostEffects(ID3D11Device* device, const char* post_effect_file_path)
{
	//�t�@�C�����o�^
	file_path = post_effect_file_path;
	//���摜
	original_frame_buffer = make_unique<FrameBuffer>(device,
		SCREEN_WIDTH, SCREEN_HEIGHT);
	//�|�X�g�G�t�F�N�g���|����摜
	post_effect_frame_buffer = make_unique<FrameBuffer>(device,
		SCREEN_WIDTH, SCREEN_HEIGHT, FB_FLAG::COLOR);

	//�ŏI�o��
	final_sprite = make_unique<FullscreenQuad>(device);

	//-----�쐣���u���[��-----//
	bloom = make_unique<Bloom>(device, SCREEN_WIDTH, SCREEN_HEIGHT);

	//�萔�o�b�t�@
	cb_post_effect = std::make_unique<Constants<CB_PostEffect>>(device);

	//�p�����[�^�[���[�h
	load_data_file();

	//�V�F�[�_�[
	create_ps_from_cso(device, "shaders/post_effects.cso", post_effects.ReleaseAndGetAddressOf());

	//�e�N�X�`���ݒ�
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	load_texture_from_file(device, L"./resources/Effects/Textures/T_Perlin_Noise_M.tga", dissolve_texture.ReleaseAndGetAddressOf(), &texture2d_desc);
	load_texture_from_file(device, L"./resources/TexMaps/distortion.tga", distortion_texture.ReleaseAndGetAddressOf(), &texture2d_desc);

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


void PostEffects::blit()
{
	Graphics& graphics = Graphics::instance();
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

	//�V�[���J�ڃf�B�]���u�e�N�X�`��
	graphics.get_dc().Get()->PSSetShaderResources(10, 1, dissolve_texture.GetAddressOf());
	graphics.get_dc().Get()->PSSetShaderResources(11, 1, dissolve_texture.GetAddressOf());
	//�|�X�g�G�t�F�N�g��������
	final_sprite->blit(graphics.get_dc().Get(), post_effect_frame_buffer->get_color_map().GetAddressOf(), 0, 1, post_effects.Get());



#if USE_IMGUI
	imgui_menu_bar("Window", "post_effect", display_post_effects_imgui);
	if (display_post_effects_imgui)
	{
		ImGui::Begin("PostEffect");
		ImGui::Text("Data");
		if (ImGui::Button("load"))
		{
			load_data_file();
		}
		ImGui::Separator();
		if (ImGui::Button("save"))
		{
			save_data_file();
		}
		//�p�����[�^�ݒ�
		if (ImGui::CollapsingHeader("Param", ImGuiTreeNodeFlags_DefaultOpen))
		{

			ImGui::DragFloat("blur_threshold", &cb_post_effect->data.bloom_extraction_threshold, 0.1f, -1.0f, 10.0f);
			ImGui::DragFloat("blur_intensity", &cb_post_effect->data.blur_convolution_intensity, 0.1f, 0, 20);
			ImGui::DragFloat("radial_power", &cb_post_effect->data.radial_power, 0.1f, 0, 1.0f);
			ImGui::DragFloat2("scene_threshold", &cb_post_effect->data.scene_threshold.x, 0.1f, 0, 1.0f);
			ImGui::DragFloat("hueShift", &cb_post_effect->data.hueShift, 0.1f, 0.0f, 10.0f);
			ImGui::DragFloat("saturation", &cb_post_effect->data.saturation, 0.1f, 0.0f, 10.0f);
			ImGui::DragFloat("brightness", &cb_post_effect->data.brightness, 0.1f, 1.0f, 10.0f);
			ImGui::DragFloat("contrast", &cb_post_effect->data.contrast, 0.1f, 0.0f, 50.0f);
			ImGui::DragFloat("falloff", &cb_post_effect->data.falloff, 0.1f, 0, 1.0f);
			ImGui::DragFloat("amount", &cb_post_effect->data.amount, 0.1f, 0, 1.0f);
			ImGui::DragFloat3("vignette_color", &cb_post_effect->data.vignette_color.x, 0.1f, 0, 1.0f);

		}
		if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("original");
			ImGui::Image(original_frame_buffer->get_color_map().Get(), { SCREEN_WIDTH * (ImGui::GetWindowSize().x / SCREEN_WIDTH),  SCREEN_HEIGHT * (ImGui::GetWindowSize().y / SCREEN_HEIGHT) });
		}


		ImGui::End();
	};
#endif

}

void PostEffects::load_data_file()
{
	// Json�t�@�C������l���擾
	std::filesystem::path path = file_path;
	path.replace_extension(".json");
	if (std::filesystem::exists(path.c_str()))
	{
		std::ifstream ifs;
		ifs.open(path);
		if (ifs)
		{
			cereal::JSONInputArchive o_archive(ifs);
			o_archive(cb_post_effect.get()->data);
		}
	}

}

void PostEffects::save_data_file()
{
	//�x�[�X�N���X�̏������p�����[�^�[�����X�V
	post_effect_init_param = cb_post_effect.get()->data;
	// Json�t�@�C������l���擾
	std::filesystem::path path = file_path;
	path.replace_extension(".json");
	std::ofstream ifs;
	ifs.open(path);
	if (ifs)
	{
		cereal::JSONOutputArchive o_archive(ifs);
		o_archive(post_effect_init_param);
	}

}

PostEffects::CB_PostEffect& PostEffects::get_now_param()
{
	CB_PostEffect param = cb_post_effect.get()->data;
	return param;
}
