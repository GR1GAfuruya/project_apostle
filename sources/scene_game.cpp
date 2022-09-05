#include "scene_game.h"

#include "device.h"
#include "imgui_include.h"
#include "scene_loading.h"
#include "scene_manager.h"
#include "stage_main.h"
#include "stage_manager.h"
SceneGame::SceneGame(Graphics& graphics)
{
	
}

void SceneGame::initialize(Graphics& graphics)
{
	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain(graphics);
	stageManager.Register(stageMain);

	camera = std::make_unique<Camera>(graphics);
	player = std::make_unique<Player>(graphics, camera.get());
	boss = std::make_unique<Boss>(graphics);
	post_effect = std::make_unique<PostEffects>(graphics.get_device().Get());
	field_spark_particle = std::make_unique<field_spark_particles>(graphics.get_device().Get(), player->get_position());
	operation_ui = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprite\\operations.png", 1);

	deferred = std::make_unique<DeferredRenderer>(graphics);
	light_manager = std::make_unique<LightManager>(graphics);
	skybox = std::make_unique<SkyBox>(graphics);
	std::shared_ptr<PointLight> p = make_shared<PointLight>(graphics, DirectX::XMFLOAT3(1, -30, 1), 30.0f, 0.0f, 1.0f,1.0f);
	light_manager->register_light(p);
	std::shared_ptr<DirectionalLight> d = make_shared<DirectionalLight>(graphics, DirectX::XMFLOAT3(1, -1, 1), 0.4f, 0.0f, 0.0f);
	light_manager->register_light(d);
}

void SceneGame::finalize()
{
	StageManager::Instance().Clear();
}

void SceneGame::update(float elapsed_time, Graphics& graphics)
{
	StageManager& stageManager = StageManager::Instance();
	camera->update(elapsed_time);
	camera->calc_view_projection(graphics, elapsed_time);
	camera->set_trakking_target(player.get()->get_camera_target_pos());
	player->update(graphics, elapsed_time, camera.get(), stage.get());
	boss->update(graphics, elapsed_time, stage.get());
	stageManager.update(elapsed_time);

	//particles->update(graphics,elapsed_time);
	Mouse& mouse = Device::instance().get_mouse();

	field_spark_particle->update(graphics.get_dc().Get(), elapsed_time, player->get_position());

	//シーンリセット（仮置き）
	if (mouse.get_button() & Mouse::BTN_ENTER)
	{
		scene_reset();
	}
}

void SceneGame::render(float elapsed_time, Graphics& graphics)
{
	///////////////////////////////////////////////////////////////////
	///						ディファ―ドレンダリング					///
	//////////////////////////////////////////////////////////////////
	StageManager& stageManager = StageManager::Instance();
	deferred->active(graphics);
	// 描画ステート設定
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::SOLID_COUNTERCLOCKWISE);
	graphics.shader_activate(SHADER_TYPE::PBR,RENDER_TYPE::Deferred);
	player->render_d(graphics,elapsed_time,camera.get());
	boss->render_d(graphics,elapsed_time);
	
	stageManager.render(elapsed_time, graphics);
	//
	deferred->deactive(graphics,*light_manager);
	//レンダーターゲットを戻す
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	post_effect->begin(graphics.get_dc().Get());
	deferred->render(graphics);

	///////////////////////////////////////////////////////////////////
	///						フォワードレンダリング					///
	//////////////////////////////////////////////////////////////////
	graphics.shader_activate(Graphics::SHADER_TYPES::LAMBERT, RENDER_TYPE::Forward);
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ADD, ST_RASTERIZER::CULL_NONE);
	//前後処理を正しく行うためディファ―ドの深度ステンシルビューを使用する
	//ポストエフェクトによりレンダーターゲットビューが変わっているので対応させる
	ID3D11RenderTargetView* render_target_views{};
	graphics.get_dc()->OMGetRenderTargets(1, &render_target_views, nullptr);
	graphics.get_dc()->OMSetRenderTargets(1, &render_target_views,	deferred->get_dsv());
	//スカイボックス
	skybox->render(graphics);
	//プレイヤー（フォワード）
	player->render_f(graphics, elapsed_time, camera.get());
	//ボス（フォワード）
	boss->render_f(graphics, elapsed_time);
	//ステージ上に舞う火花
	field_spark_particle->render(graphics.get_dc().Get());
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	//ポストエフェクト用のレンダーここまで
	post_effect->end(graphics.get_dc().Get());

	post_effect->blit(graphics);
	//UI
	operation_ui->begin(graphics.get_dc().Get());
	operation_ui->render(graphics.get_dc().Get(), { 0, 10 }, { 2, 2 });
	operation_ui->end(graphics.get_dc().Get());	debug_gui();
	
}

void SceneGame::debug_gui()
{
	camera->debug_gui();
}

void SceneGame::scene_reset()
{
	player->initialize();
	boss->initialize();
}
