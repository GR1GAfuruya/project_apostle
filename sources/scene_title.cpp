#include "scene_title.h"
#include "scene_loading.h"
#include "scene_game.h"
#include "scene_manager.h"
#include "device.h"
#include "texture.h"
SceneTitle::SceneTitle(Graphics& graphics)
{
	sprite_title_back = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".//resources//Sprite//Title//title_back.png", 1);
	sprite_title_logo = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".//resources//Sprite//Title//title_logo.png", 1);
	sprite_title_logo_back = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".//resources//Sprite//Title//title_logo_back.png", 1);
	sprite_start = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".//resources//Sprite//Title//gamestart.png", 1);
	sprite_exit = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".//resources//Sprite//Title//exit.png", 1);
	select_bar = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".//resources//Sprite//Title//select_bar.png", 1);

	D3D11_TEXTURE2D_DESC texture2d_desc{};
	load_texture_from_file(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill3_output.png", srv_main_color.ReleaseAndGetAddressOf(), &texture2d_desc);
	load_texture_from_file(graphics.get_device().Get(), L"./resources/Effects/Textures/T_Perlin_Noise_M.tga", srv_mask.ReleaseAndGetAddressOf(), &texture2d_desc);
	load_texture_from_file(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga", srv_distortion.ReleaseAndGetAddressOf(), &texture2d_desc);

	create_ps_from_cso(graphics.get_device().Get(), "./shaders/title_logo.cso", logo_ps.ReleaseAndGetAddressOf());

	is_change_scean_start = false;
	camera = std::make_unique<Camera>(graphics, "./resources/Data/scene_title/title_post_effect.json");

	//セレクトバー
	selected_menu_state = TITLE_MENU::GAME_START;
	select_bar_pos = { 400.0f,400.0f };
}

void SceneTitle::initialize(Graphics& graphics)
{
}

void SceneTitle::finalize()
{
}

void SceneTitle::update(float elapsedTime, Graphics& graphics)
{
	//カメラ更新
	camera->update(elapsedTime);

	camera->calc_view_projection(graphics, elapsedTime);
	//デバイス
	Mouse& mouse = Device::instance().get_mouse();
	GamePad& game_pad = Device::instance().get_game_pad();

	//メニューセレクト
	if (game_pad.get_axis_LY() > 0.2f)
	{
		//上に倒したときはゲームスタート
		selected_menu_state = TITLE_MENU::GAME_START;
	}
	else if(game_pad.get_axis_LY() < -0.2f)
	{
		//下に倒したときは抜ける
		selected_menu_state = TITLE_MENU::EXIT;
	}


	//ボタンを押したときの挙動
	switch (selected_menu_state)
	{
	case SceneTitle::TITLE_MENU::GAME_START:
		if (mouse.get_button() & mouse.BTN_Z || game_pad.get_button() & game_pad.BTN_A)
		{
			is_change_scean_start = true;
		}
		break;
	case SceneTitle::TITLE_MENU::EXIT:
		if (mouse.get_button() & mouse.BTN_Z || game_pad.get_button() & game_pad.BTN_A)
		{
			PostQuitMessage(0);
		}
		break;
	default:
		break;
	}

	if (is_change_scean_start)
	{
		PostEffects::CB_PostEffect nowparam = camera.get()->get_post_effect()->get_now_param();
		nowparam.scene_threshold.x += elapsedTime;
		camera->get_post_effect()->set_posteffect_param(nowparam);
		if (nowparam.scene_threshold.x >= 1.0f)
		{
			SceneManager::instance().change_scene(graphics, new SceneLoading(new SceneGame(graphics)));
			return;
		}
	}

}

void SceneTitle::render(float elapsedTime,Graphics& graphics)
{

	const DirectX::XMFLOAT2 title_pos = { 130,100 };
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);

	camera->get_post_effect()->begin(graphics.get_dc().Get());
	//タイトル背景
	{
		sprite_title_back->begin(graphics.get_dc().Get());
		sprite_title_back->render(graphics.get_dc().Get(), { 0, 0 }, { 1,1 });
		sprite_title_back->end(graphics.get_dc().Get());
	}

	//タイトルロゴ
	{
		ID3D11ShaderResourceView* srv[3] = {
		srv_main_color.Get() ,
		srv_mask.Get() ,
		srv_distortion.Get() };
		graphics.get_dc().Get()->PSSetShaderResources(
			20, 3, srv);

		sprite_title_logo_back->begin(graphics.get_dc().Get(), logo_ps.Get());
		sprite_title_logo_back->render(graphics.get_dc().Get(), title_pos, { 1,1 });
		sprite_title_logo_back->end(graphics.get_dc().Get());


		sprite_title_logo->begin(graphics.get_dc().Get());
		sprite_title_logo->render(graphics.get_dc().Get(), title_pos, { 1,1 });
		sprite_title_logo->end(graphics.get_dc().Get());
	}
	
	const DirectX::XMFLOAT2 start_str_pos = { 400.0f,440.0f };//メニューテキストのGAME_STARTの位置
	const DirectX::XMFLOAT2 exit_str_pos = { 400.0f,540.0f };//メニューテキストのEXITの位置
	const float offset = -30.0f;//メニューバーの位置に足す値
	//メニュー選択バー
	{
		float rate = 5.0f * elapsedTime;
		DirectX::XMFLOAT2 target_pos;
		switch (selected_menu_state)
		{
		case SceneTitle::TITLE_MENU::GAME_START:
			target_pos = { start_str_pos.x, start_str_pos.y + offset };
			break;
		case SceneTitle::TITLE_MENU::EXIT:
			target_pos = { exit_str_pos.x, exit_str_pos.y + offset };
			break;
		}
		select_bar_pos = Math::lerp(select_bar_pos, target_pos, rate);

		select_bar->begin(graphics.get_dc().Get());
		select_bar->render(graphics.get_dc().Get(), select_bar_pos, { 1,1 });
		select_bar->end(graphics.get_dc().Get());

	}
	//メニューテキスト
	{
		sprite_start->begin(graphics.get_dc().Get());
		sprite_start->render(graphics.get_dc().Get(), start_str_pos, { 1,1 });
		sprite_start->end(graphics.get_dc().Get());

		const DirectX::XMFLOAT2 exit_pos = { exit_str_pos };
		sprite_exit->begin(graphics.get_dc().Get());
		sprite_exit->render(graphics.get_dc().Get(), exit_pos, { 1,1 });
		sprite_exit->end(graphics.get_dc().Get());
	}

	
	//***************************************************************//
	///						ポストエフェクト  				        ///
	//***************************************************************//
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	camera->get_post_effect()->end(graphics.get_dc().Get());
	camera->get_post_effect()->blit(graphics);

//#if USE_IMGUI
//	ImGui::Begin("sprite");
//	//ImGui::DragFloat2("pos",&start_str_pos.x,0.1f);
//	ImGui::DragFloat("offset",&offset,0.1f);
//	ImGui::End();
//
//
//#endif

}
