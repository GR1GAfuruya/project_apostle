#include "scene_title.h"
#include "scene_loading.h"
#include "scene_game.h"
#include "scene_tutorial.h"
#include "scene_manager.h"
#include "device.h"
#include "texture.h"
SceneTitle::SceneTitle(Graphics& graphics)
{
	title_back = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".//resources//Sprite//Title//title_back.png", 1);
	title_logo = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".//resources//Sprite//Title//title_logo.png", 1);
	title_logo_back = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".//resources//Sprite//Title//title_logo_back.png", 1);

	D3D11_TEXTURE2D_DESC texture2d_desc{};
	load_texture_from_file(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill3_output.png", srv_main_color.ReleaseAndGetAddressOf(), &texture2d_desc);
	load_texture_from_file(graphics.get_device().Get(), L"./resources/Effects/Textures/T_Perlin_Noise_M.tga", srv_mask.ReleaseAndGetAddressOf(), &texture2d_desc);
	load_texture_from_file(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga", srv_distortion.ReleaseAndGetAddressOf(), &texture2d_desc);

	create_ps_from_cso(graphics.get_device().Get(), "./shaders/title_logo.cso", logo_ps.ReleaseAndGetAddressOf());


	camera = std::make_unique<Camera>(graphics);
}

void SceneTitle::initialize(Graphics& graphics)
{
}

void SceneTitle::finalize()
{
}

void SceneTitle::update(float elapsedTime, Graphics& graphics)
{

	camera->calc_view_projection(graphics, elapsedTime);
	Mouse& mouse = Device::instance().get_mouse();
	GamePad& game_pad = Device::instance().get_game_pad();
	if (mouse.get_button() & mouse.BTN_Z || game_pad.get_button() & game_pad.BTN_A)
	{
		SceneManager::instance().change_scene(graphics,new SceneLoading(new SceneGame(graphics)));
	}

}

void SceneTitle::render(float elapsedTime,Graphics& graphics)
{

	static DirectX::XMFLOAT2 pos = { 300,100 };
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);

	//camera->get_post_effect()->begin(graphics.get_dc().Get());
	title_back->begin(graphics.get_dc().Get());
	title_back->render(graphics.get_dc().Get(), { 0, 0 }, { 1,1 });
	title_back->end(graphics.get_dc().Get());


	ID3D11ShaderResourceView* srv[3] = {
	srv_main_color.Get() ,
	srv_mask.Get() ,
	srv_distortion.Get() };
	graphics.get_dc().Get()->PSSetShaderResources(
		20,3, srv);

	title_logo_back->begin(graphics.get_dc().Get(), logo_ps.Get());
	title_logo_back->render(graphics.get_dc().Get(), pos, { 1,1 });
	title_logo_back->end(graphics.get_dc().Get());


	title_logo->begin(graphics.get_dc().Get());
	title_logo->render(graphics.get_dc().Get(), pos, { 1,1 });
	title_logo->end(graphics.get_dc().Get());

	//***************************************************************//
	///						ポストエフェクト  				        ///
	//***************************************************************//
	//graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	//camera->get_post_effect()->end(graphics.get_dc().Get());
	//camera->get_post_effect()->blit(graphics);

//#if USE_IMGUI
//	ImGui::Begin("sprite");
//	ImGui::DragFloat2("pos",&pos.x,0.1f);
//	ImGui::End();
//
//
//#endif

}
