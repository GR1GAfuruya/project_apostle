#include "scene_tutorial.h"
#include "device.h"
#include "imgui_include.h"
#include "scene_loading.h"
#include "scene_manager.h"
#include "stage_main.h"
#include "stage_manager.h"
#include "scene_game.h"
#include "material_manager.h"
SceneTutorial::SceneTutorial(Graphics& graphics)
{
}

void SceneTutorial::initialize(Graphics& graphics)
{
	//マテリアルのロード
	MaterialManager::instance().create_materials(graphics);
	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain(graphics);
	stageManager.Register(stageMain);

	camera = std::make_unique<Camera>(graphics);
	player = std::make_unique<Player>(graphics, camera.get());
	//post_effect = std::make_unique<PostEffects>(graphics.get_device().Get());
	operation_ui = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprite\\UI\\operations.png", 1);

	deferred = std::make_unique<DeferredRenderer>(graphics);

	tutorial = std::make_unique<Tutorial>(graphics);
	tutorial->initialize(graphics);

	LightManager::instance().initialize(graphics);
	skybox = std::make_unique<SkyBox>(graphics);
}

void SceneTutorial::finalize()
{
	StageManager::Instance().Clear();
}

void SceneTutorial::update(float elapsed_time, Graphics& graphics)
{
	GamePad& gamepad = Device::instance().get_game_pad();
	if (gamepad.get_button_down() & GamePad::BTN_BACK)
	{
		tutorial->set_is_tutorial(!tutorial->get_is_tutorial());
	}

	if (tutorial->get_is_tutorial())
	{
		tutorial->update(graphics, elapsed_time);
		return;
	}
	//**********カメラの更新**********//
	camera->update(elapsed_time);
	camera->calc_view_projection(graphics, elapsed_time);
	camera->set_trakking_target(player.get()->get_gazing_point());

	//カメラの経過時間
	float camera_elapsed_time = elapsed_time;
	//ヒットストップ時の経過時間処理
	//if (camera->get_camera_stop())
	//{
	//	//経過時間を0に
	//	camera_elapsed_time = 0;
	//}
	//**********プレイヤーの更新**********//
	player->update(graphics, camera_elapsed_time, camera.get());

	/*player->calc_collision_vs_enemy(boss->get_body_collision().capsule, boss->get_body_collision().height);

	player->calc_attack_vs_enemy(boss->get_body_collision().capsule, boss->damaged_function, camera.get());

	player->judge_skill_collision(boss->get_body_collision().capsule, boss->damaged_function, camera.get());*/

	//**********ボスの更新**********//

	//**********ステージの更新**********//
	StageManager::Instance().update(elapsed_time);

	Mouse& mouse = Device::instance().get_mouse();
	GamePad& game_pad = Device::instance().get_game_pad();
	if (mouse.get_button() & mouse.BTN_Z || game_pad.get_button() & game_pad.BTN_X)
	{
		SceneManager::instance().change_scene(graphics, new SceneLoading(new SceneGame(graphics)));
	}

}

void SceneTutorial::render(float elapsed_time, Graphics& graphics)
{
	StageManager& stageManager = StageManager::Instance();

	//***************************************************************//
	///						シャドウをかけるモデルを描画			///
	//***************************************************************//
#if CAST_SHADOW
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::SOLID_COUNTERCLOCKWISE);
	graphics.shader_activate(SHADER_TYPE::SHADOW, RENDER_TYPE::Deferred);
	deferred->shadow_active(graphics, player->get_position());
	//ステージ描画
//	stageManager.shadow_render(graphics,elapsed_time);

	//プレイヤー描画
	player->render_d(graphics, elapsed_time, camera.get());

	//ボス描画
	//boss->render_d(graphics, elapsed_time);

	deferred->shadow_deactive(graphics);
#endif
	//***************************************************************//
	///						ディファ―ドレンダリング				///
	//***************************************************************//
	deferred->active(graphics);
	// 描画ステート設定
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::SOLID_COUNTERCLOCKWISE);
	graphics.shader_activate(SHADER_TYPE::PBR, RENDER_TYPE::Deferred);

	//ステージ描画
	stageManager.render(graphics, elapsed_time, camera.get());

	//プレイヤー描画
	player->render_d(graphics, elapsed_time, camera.get());

	//ボス描画


	//ここで各種ライティング（環境光、平行光、点光源）
	deferred->deactive(graphics);

	//レンダーターゲットを戻す
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ADD, ST_RASTERIZER::CULL_NONE);
	camera->get_post_effect()->begin(graphics.get_dc().Get());
	deferred->render(graphics);

	//***************************************************************//
	///						フォワードレンダリング					///
	//***************************************************************//
	graphics.shader_activate(Graphics::SHADER_TYPES::LAMBERT, RENDER_TYPE::Forward);
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	//前後処理を正しく行うためディファ―ドの深度ステンシルビューを使用する
	//ポストエフェクトによりレンダーターゲットビューが変わっているので対応させる
	ID3D11RenderTargetView* render_target_views{};
	graphics.get_dc()->OMGetRenderTargets(1, &render_target_views, nullptr);
	graphics.get_dc()->OMSetRenderTargets(1, &render_target_views, deferred->get_dsv());
	//スカイボックス
	//skybox->render(graphics);
	//ボス（フォワード）

	//プレイヤー（フォワード）
	player->render_f(graphics, elapsed_time, camera.get());

	//テストエフェクト更新
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::SOLID_ONESIDE);

	//***************************************************************//
	///						ポストエフェクト  				        ///
	//***************************************************************//
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	camera->get_post_effect()->end(graphics.get_dc().Get());
	camera->get_post_effect()->blit(graphics);

	//***************************************************************//
	///						        UI  				            ///
	//***************************************************************//

	//boss->render_ui(graphics, elapsed_time);
	player->render_ui(graphics, elapsed_time);
	camera->debug_gui();


	//チュートリアル描画
	tutorial->render(graphics.get_dc().Get());

	//デバッグレンダー
	graphics.get_dc()->OMGetRenderTargets(1, &render_target_views, nullptr);
	graphics.get_dc()->OMSetRenderTargets(1, &render_target_views, deferred->get_dsv());
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::WIREFRAME_CULL_BACK);
	debug_figure->render_all_figures(graphics.get_dc().Get());

}

void SceneTutorial::debug_gui()
{
}

void SceneTutorial::scene_reset()
{
}
