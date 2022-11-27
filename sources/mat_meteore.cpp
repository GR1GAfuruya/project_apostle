#include "mat_meteore.h"

MeteoreMat::MeteoreMat(Graphics& graphics)
{
	//シェーダーリソースの設定
	register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill3_output.png");
	register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/T_Perlin_Noise_M.tga");
	register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga");
	//ピクセルシェーダーの設定
	create_pixel_shader(graphics.get_device().Get(), "./shaders/meteore_core.cso");

	//ID設定
	id = "meteore";
}
