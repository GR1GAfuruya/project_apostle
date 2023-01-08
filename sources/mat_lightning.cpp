#include "mat_lightning.h"

LightningMat::LightningMat(Graphics& graphics)
{
	//シェーダーリソースの設定
	register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Lightning_output.png");
	register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/T_Perlin_Noise_M.tga");
	//ピクセルシェーダーの設定
	create_pixel_shader(graphics.get_device().Get(), "./shaders/lightning.cso");

	//ID設定
	id = "lightning";
}
