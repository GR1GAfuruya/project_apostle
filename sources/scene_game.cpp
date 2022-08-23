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
	post_effect = std::make_unique<PostEffects>(graphics.get_device().Get());
	gpu_particle = std::make_unique<GPU_Particles>(graphics.get_device().Get(),600000);
	gpu_particle->initialize(graphics.get_dc().Get());
	field_spark_particle = std::make_unique<field_spark_particles>(graphics.get_device().Get(), player->get_position());
	//particles = std::make_unique<Particles>(graphics);
	deferred = std::make_unique<DeferredRenderer>(graphics);
	light_manager = std::make_unique<LightManager>(graphics);
	skybox = std::make_unique<SkyBox>(graphics);

	std::shared_ptr<PointLight> p = make_shared<PointLight>(graphics, DirectX::XMFLOAT3(-19, 5, 12), 30, 0, 1, 0);
	light_manager->register_light(p);
	for (int i = 0; i < 1; i++)
	{
		std::shared_ptr<PointLight> p = make_shared<PointLight>(graphics, DirectX::XMFLOAT3(rand() % 100 - 50,
			5,
			rand() % 100 - 50),
			30,
			rand() % 500 * 0.01f,
			rand() % 500 * 0.01f,
			rand() % 500 * 0.01f);
		light_manager->register_light(p);
	}
	std::shared_ptr<DirectionalLight> d = make_shared<DirectionalLight>(graphics, DirectX::XMFLOAT3(1, -1, 1), 1.0f, 0.0f, 0.0f);
	light_manager->register_light(d);
	//particles->initialize(graphics);
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
	player->update(graphics, elapsed_time, camera.get(),stage.get());
	stageManager.update( elapsed_time);

	gpu_particle->update(graphics.get_dc().Get(),elapsed_time);
	//particles->update(graphics,elapsed_time);
		Mouse& mouse = Device::instance().get_mouse();
		gpu_particle->particle_constants->data.emitter.pos = player->get_camera_target_pos();

		field_spark_particle->integrate(graphics.get_dc().Get(), elapsed_time, player->get_position());
	if (mouse.get_button() & mouse.BTN_M)
	{
		gpu_particle->particle_constants->data.emitter.pos.x += cos(gpu_particle->particle_constants->data.emitter.emit_time++ * 5) * 5;
		gpu_particle->particle_constants->data.emitter.pos.z += cos(gpu_particle->particle_constants->data.emitter.emit_time++ * 5) * 5;
		//gpu_particle->launch_emitter(graphics.get_dc().Get(),0);
		/*particles->set_emit_param(0, 0, { 10,10 });
		particles->emit();*/
	}

	if (mouse.get_button_up() & mouse.BTN_ENTER)
	{
		
		gpu_particle->particle_constants->data.emitter.emit_time = 0;
	}
#if USE_IMGUI
	if (ImGui::Button("emit"))
	{
		gpu_particle->particle_emit(graphics.get_dc().Get());
	}
	

#endif // 0

	if (mouse.get_button() & mouse.BTN_Z)
	{
		//SceneManager::instance().change_scene(graphics, new SceneLoading(new SceneTest(graphics)));
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
	
	stageManager.render(elapsed_time, graphics);
	//graphics.set_graphic_state_priset(DEPTH_ST::ZT_ON_ZW_OFF, BLEND_ST::ALPHA, RASTERIZER_ST::CULL_NONE);
	//particles->render(graphics);
	
	
	deferred->deactive(graphics,*light_manager);
	//レンダーターゲットを戻す
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::SOLID_ONESIDE);
	post_effect->begin(graphics.get_dc().Get());
	deferred->render(graphics);

	///////////////////////////////////////////////////////////////////
	///						フォワードレンダリング					///
	//////////////////////////////////////////////////////////////////
	graphics.shader_activate(Graphics::SHADER_TYPES::LAMBERT, RENDER_TYPE::Forward);
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	//前後処理を正しく行うためディファ―ドの深度ステンシルビューを使用する
	//ポストエフェクトによりレンダーターゲットビューが変わっているので対応させる
	ID3D11RenderTargetView* render_target_views{};
	graphics.get_dc()->OMGetRenderTargets(1, &render_target_views, nullptr);
	graphics.get_dc()->OMSetRenderTargets(1, &render_target_views,
		deferred->get_dsv());

	//skybox->render(graphics);
	graphics.set_depth_state(ST_DEPTH::ZT_ON_ZW_OFF);
	player->render_f(graphics, elapsed_time, camera.get());
	gpu_particle->render(graphics.get_dc().Get(),graphics.get_device().Get());
	field_spark_particle->render(graphics.get_dc().Get());
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::SOLID_ONESIDE);
	post_effect->end(graphics.get_dc().Get());

	post_effect->blit(graphics);
	debug_gui();
	
}

void SceneGame::debug_gui()
{
#ifdef USE_IMGUI
	ImGui::Begin("Camera");
	ImGui::DragFloat3("pos", &chara_pos.x);
	ImGui::End();

	/*ImGui::Begin("effect");
	if (ImGui::Button("play"))
	{
	}
	if (ImGui::Button("stop"))
	{
	}
	if (sample_effect)
	{
		static  float angle = 0.0f;
		static  DirectX::XMFLOAT3 pos = {};
		ImGui::DragFloat("effec_angle", &angle);
		ImGui::DragFloat3("effec_pos", &pos.x);
		sample_effect->set_angle(angle);
		sample_effect->set_position(pos);
	}
	ImGui::End();*/
#endif // USE_IMGUI
	camera->debug_gui();
}
