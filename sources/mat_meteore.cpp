#include "mat_meteore.h"

MeteoreMat::MeteoreMat(Graphics& graphics)
{
	//�V�F�[�_�[���\�[�X�̐ݒ�
	register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill3_output.png");
	register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/T_Perlin_Noise_M.tga");
	register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga");
	//�s�N�Z���V�F�[�_�[�̐ݒ�
	create_pixel_shader(graphics.get_device().Get(), "./shaders/meteore_core.cso");

	//ID�ݒ�
	id = "meteore";
}