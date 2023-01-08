#include "mat_lightning.h"

LightningMat::LightningMat(Graphics& graphics)
{
	//�V�F�[�_�[���\�[�X�̐ݒ�
	register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Lightning_output.png");
	register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/T_Perlin_Noise_M.tga");
	//�s�N�Z���V�F�[�_�[�̐ݒ�
	create_pixel_shader(graphics.get_device().Get(), "./shaders/lightning.cso");

	//ID�ݒ�
	id = "lightning";
}
