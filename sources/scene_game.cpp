#include "scene_game.h"

#include "device.h"
#include "imgui_include.h"
#include "scene_loading.h"
#include "scene_manager.h"
#include "stage_main.h"
#include "stage_manager.h"
#include "scene_title.h"
#include "material_manager.h"
SceneGame::SceneGame(Graphics& graphics)
{
	
}
//==============================================================
// 
//初期化
// 
//==============================================================
void SceneGame::initialize(Graphics& graphics)
{
	//マテリアルのロード
	MaterialManager::instance().create_materials(graphics);
	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain(graphics);
	stageManager.Register(stageMain);

	camera = std::make_unique<Camera>(graphics);
	player = std::make_unique<Player>(graphics, camera.get());
	boss = std::make_unique<Boss>(graphics);
	camera->set_lock_on_target(boss.get()->get_gazing_point());
	//post_effect = std::make_unique<PostEffects>(graphics.get_device().Get());
	field_spark_particle = std::make_unique<field_spark_particles>(graphics.get_device().Get(), player->get_position());
	operation_ui = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprite\\UI\\operations.png", 1);

	deferred = std::make_unique<DeferredRenderer>(graphics);

	tutorial = std::make_unique<Tutorial>(graphics);
	tutorial->initialize(graphics);

	LightManager::instance().initialize(graphics);
	skybox = std::make_unique<SkyBox>(graphics);
	dir_light = make_shared<DirectionalLight>(graphics, DirectX::XMFLOAT3(0.6f, -0.6f, 1.6f), DirectX::XMFLOAT3(0.4f, 0.1f, 0.0f));
	LightManager::instance().register_light("scene_dir", dir_light);

	//テスト用
#if _DEBUG
	 test_mesh_effect = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/lightning.fbx");
	 test_mesh_effect->set_material(MaterialManager::instance().mat_lightning.get());
	 test_mesh_effect->set_scale(0.1f);

	 test_emitter = std::make_unique<Emitter>(graphics,200);
#endif
}

void SceneGame::finalize()
{
	StageManager::Instance().Clear();
}
//==============================================================
// 
//更新
// 
//==============================================================
void SceneGame::update(float elapsed_time, Graphics& graphics)
{
	//ゲームパッド
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

	player->calc_collision_vs_enemy(boss->get_body_collision().capsule, boss->get_body_collision().height);
	
	player->calc_attack_vs_enemy(boss->get_body_collision().capsule, boss->damaged_function, camera.get());
	
	player->judge_skill_collision(boss->get_body_collision().capsule, boss->damaged_function, camera.get());

	//**********ボスの更新**********//
	boss->update(graphics, camera_elapsed_time, camera.get());
	
	//ボスの攻撃対象を設定
	boss->set_location_of_attack_target(player->get_position());
	boss->calc_attack_vs_player(player->collider.start, player->collider.end, player->collider.radius, player->damaged_function);
	
	//**********ステージの更新**********//
	StageManager::Instance().update(elapsed_time);

	//particles->update(graphics,elapsed_time);

	//エフェクト更新
#if _DEBUG
	test_mesh_effect->set_life_span(5);
	test_mesh_effect->update(graphics,elapsed_time);
	test_mesh_effect->set_is_loop(true);

	test_emitter->update(graphics, elapsed_time);
	//test_meteore->update(graphics, elapsed_time);
#endif
	field_spark_particle->update(graphics.get_dc().Get(), elapsed_time, player->get_position());
}

//==============================================================
// 
//描画
// 
//==============================================================
void SceneGame::render(float elapsed_time, Graphics& graphics)
{
	StageManager& stageManager = StageManager::Instance();

	//***************************************************************//
	///						シャドウをかけるモデルを描画			///
	//***************************************************************//
#if CAST_SHADOW
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::SOLID_COUNTERCLOCKWISE);
	graphics.shader_activate(SHADER_TYPE::SHADOW, RENDER_TYPE::Deferred);
	deferred->shadow_active(graphics, *light_manager);
	//ステージ描画
	stageManager.render(elapsed_time, graphics);

	//プレイヤー描画
	player->render_d(graphics, elapsed_time, camera.get());

	//ボス描画
	boss->render_d(graphics, elapsed_time);

	deferred->shadow_deactive(graphics);
#endif
	//***************************************************************//
	///						ディファ―ドレンダリング				///
	//***************************************************************//
	deferred->active(graphics);
	// 描画ステート設定
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::SOLID_COUNTERCLOCKWISE);
	graphics.shader_activate(SHADER_TYPE::PBR,RENDER_TYPE::Deferred);
	
	//ステージ描画
	stageManager.render(elapsed_time, graphics);

	//プレイヤー描画
	player->render_d(graphics,elapsed_time,camera.get());

	//ボス描画
	boss->render_d(graphics,elapsed_time);


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
	graphics.get_dc()->OMSetRenderTargets(1, &render_target_views,	deferred->get_dsv());
	//スカイボックス
	//skybox->render(graphics);
	//ボス（フォワード）
	boss->render_f(graphics, elapsed_time);
	//プレイヤー（フォワード）
	player->render_f(graphics, elapsed_time, camera.get());
	//ステージ上に舞う火花
	field_spark_particle->render(graphics.get_dc().Get());


	//テスト用
#if _DEBUG
	test_emitter->debug_gui(graphics, "emitter_test");
	test_emitter->render(graphics, *camera);
	static DirectX::XMFLOAT4 test_effect_color = { 4.3f,1.0f,0.2f,1.0f };
	static DirectX::XMFLOAT3 test_effect_pos = { 0.0f,0.0f,0.0f };
	static DirectX::XMFLOAT3 test_effect_scale = { 0.1f,0.1f,0.1f };
	test_mesh_effect->render(graphics);
	test_mesh_effect->debug_gui("test_effect");
	test_mesh_effect->set_position(test_effect_pos);
	test_mesh_effect->set_scale(test_effect_scale);
	#if USE_IMGUI
	static bool display_test_effect_imgui = false;
	imgui_menu_bar("Effects", "test_effect", display_test_effect_imgui);
	if (display_test_effect_imgui)
	{
		ImGui::Begin("test_effect");
		if (ImGui::Button("test_effect_play"))
		{
			test_mesh_effect->play(test_effect_pos);
		}
		ImGui::DragFloat3("pos", &test_effect_pos.x);
		ImGui::DragFloat3("color", &test_effect_color.x, 0.1f);
		ImGui::DragFloat3("scale", &test_effect_scale.x, 0.1f);
		ImGui::End();
	}
	test_mesh_effect->constants->data.particle_color = test_effect_color;

	static DirectX::XMFLOAT3 test_meteore_pos = { 0.0f,10.0f,0.0f };
	static DirectX::XMFLOAT3 test_meteore_dir = { 0.0f,10.0f,0.0f };
	static float test_meteore_speed = 0.0f;

	//test_meteore->render(graphics);

	#endif

#endif

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
	//仮置きの操作説明
	/*operation_ui->begin(graphics.get_dc().Get());
	operation_ui->render(graphics.get_dc().Get(), { 0, 10 }, { 2, 2 });
	operation_ui->end(graphics.get_dc().Get());	*/

	boss->render_ui(graphics,elapsed_time);
	player->render_ui(graphics,elapsed_time);
	camera->debug_gui();


	//チュートリアル描画
	tutorial->render(graphics.get_dc().Get());

	//デバッグレンダー
	graphics.get_dc()->OMGetRenderTargets(1, &render_target_views, nullptr);
	graphics.get_dc()->OMSetRenderTargets(1, &render_target_views, deferred->get_dsv());
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	debug_figure->render_all_figures(graphics.get_dc().Get());
	

#if USE_IMGUI

	imgui_menu_bar("Game", "scene_game", display_imgui);
	if (display_imgui)
	{
		if (ImGui::Button("back_title"))
		{
			//シーンリセット
			SceneManager::instance().change_scene(graphics, new SceneLoading(new SceneTitle(graphics)));
			return;
		};


	}
#endif

}

void SceneGame::debug_gui()
{
	
}

void SceneGame::scene_reset()
{
	player->initialize();
	boss->initialize();
}
