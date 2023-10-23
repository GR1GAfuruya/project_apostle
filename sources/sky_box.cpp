#include "sky_box.h"
#include"user.h"
#include "texture.h"
SkyBox::SkyBox()
{
	Graphics& graphics = Graphics::instance();
	sky_model = make_unique<SkeletalMesh>(graphics.get_device().Get(), "./resources/Model/sky.fbx");
	scale = { 0.5f,0.5f,0.5f };
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	load_texture_from_file(graphics.get_device().Get(), L"./resources/envmap_miramar.png",
		env_map.ReleaseAndGetAddressOf(), &texture2d_desc);
}

void SkyBox::render()
{
	Graphics& graphics = Graphics::instance();
	graphics.get_dc()->PSSetShaderResources(15, 1, env_map.GetAddressOf());
	graphics.shader->render(graphics.get_dc().Get(), sky_model.get(), Math::calc_world_matrix(scale, angle, position));
}

void SkyBox::load_convert_env_map_and_cube_map()
{
	//// Unfiltered environment cube map (temporary).
	//Texture envTextureUnfiltered = createTextureCube(1024, 1024, DXGI_FORMAT_R16G16B16A16_FLOAT);
	//createTextureUAV(envTextureUnfiltered, 0);

	//// Load & convert equirectangular environment map to a cubemap texture.
	//{
	//	ComputeProgram equirectToCubeProgram = createComputeProgram(compileShader("shaders/hlsl/equirect2cube.hlsl", "main", "cs_5_0"));
	//	Texture envTextureEquirect = createTexture(Image::fromFile("environment.hdr"), DXGI_FORMAT_R32G32B32A32_FLOAT, 1);

	//	graphics.get_dc()->CSSetShaderResources(0, 1, envTextureEquirect.srv.GetAddressOf());
	//	graphics.get_dc()->CSSetUnorderedAccessViews(0, 1, envTextureUnfiltered.uav.GetAddressOf(), nullptr);
	//	graphics.get_dc()->CSSetSamplers(0, 1, m_computeSampler.GetAddressOf());
	//	graphics.get_dc()->CSSetShader(equirectToCubeProgram.computeShader.Get(), nullptr, 0);
	//	graphics.get_dc()->Dispatch(envTextureUnfiltered.width / 32, envTextureUnfiltered.height / 32, 6);
	//	graphics.get_dc()->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	//}

	//graphics.get_dc()->GenerateMips(envTextureUnfiltered.srv.Get());

}

