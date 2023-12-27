#include "scene_game.h"

#include "device.h"
#include "imgui_include.h"
#include "scene_loading.h"
#include "scene_manager.h"
#include "stage_main.h"
#include "stage_manager.h"
#include "scene_title.h"
#include "material_manager.h"
#include "texture.h"
#include "Operators.h"
#include "game_object.h"
#include "effect_manager.h"
SceneGame::SceneGame()
{

}
//==============================================================
// 
//������
// 
//==============================================================
void SceneGame::initialize()
{
	Graphics& graphics = Graphics::instance();
	//�}�e���A���̃��[�h
	MaterialManager::instance().create_materials();
	// �X�e�[�W������
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);

	camera = std::make_unique<Camera>("./resources/Data/scene_game/game_post_effect.json");
	player = std::make_unique<Player>(camera.get());
	boss = std::make_unique<Boss>();
	//post_effect = std::make_unique<PostEffects>(graphics.get_device().Get());
	field_spark_particle = std::make_unique<field_spark_particles>(graphics.get_device().Get(), player->get_position());
	operation_ui = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprite\\UI\\operations.png", 1);

	deferred = std::make_unique<DeferredRenderer>();

	tutorial = std::make_unique<Tutorial>();
	tutorial->initialize();
	LightManager::instance().initialize();
	skybox = std::make_unique<SkyBox>();
	dir_light = make_shared<DirectionalLight>(DirectX::XMFLOAT3(0.6f, -0.6f, 1.6f), DirectX::XMFLOAT3(0.4f, 0.1f, 0.0f));
	LightManager::instance().register_light("scene_dir", dir_light);



	//�e�X�g�p
#if _DEBUG
	test_mesh_effect = std::make_unique<MeshEffect>("./resources/Effects/Meshes/eff_sphere.fbx");
	test_mesh_effect2 = std::make_unique<MeshEffect>("./resources/Effects/Meshes/eff_tornado4.fbx");
	test_mesh_effect3 = std::make_unique<MeshEffect>("./resources/Effects/Meshes/eff_tornado.fbx");
	test_mesh_effect->set_material(MaterialManager::instance().mat_fire_distortion.get());
	test_mesh_effect->set_init_scale(0.1f);
	test_mesh_effect->set_init_color({ 4.0f, 1.0f, 0.7f, 0.8f });

	test_mesh_effect2->set_material(MaterialManager::instance().mat_fire_distortion.get());
	test_mesh_effect2->set_init_scale(0.1f);
	test_mesh_effect2->set_init_color({ 4.0f, 1.0f, 0.7f, 0.8f });

	test_mesh_effect3->set_material(MaterialManager::instance().mat_fire_distortion.get());
	test_mesh_effect3->set_init_scale(0.1f);
	test_mesh_effect3->set_init_color({ 4.0f, 1.0f, 0.7f, 0.8f });

	test_effect = make_unique<Effect>("");
	//SpriteEmitterInstance::InitParam init;
	//init.duration = 5;
	//test_emitter = std::make_unique<SpriteEmitterInstance>(init, 1000);
#endif
}

void SceneGame::finalize()
{
	StageManager::Instance().Clear();
}
//==============================================================
// 
//�X�V
// 
//==============================================================
void SceneGame::update(float elapsed_time)
{
	Graphics& graphics = Graphics::instance();
	//�Q�[���p�b�h
	GamePad& gamepad = Device::instance().get_game_pad();
	if (gamepad.get_button_down() & GamePad::BTN_BACK)
	{
		tutorial->set_is_tutorial(!tutorial->get_is_tutorial());
	}

	if (tutorial->get_is_tutorial())
	{
		tutorial->update(elapsed_time);
		return;
	}
	//**********�J�����̍X�V**********//
	camera->update(elapsed_time);
	camera->calc_view_projection(elapsed_time);
	camera->set_trakking_target(player.get()->get_gazing_point());
	//camera->set_lock_on_target(boss.get()->get_position());

	//�J�����̌o�ߎ���
	float camera_elapsed_time = camera->hit_stop_update(elapsed_time);

	//**********�v���C���[�̍X�V**********//
	player->update(camera_elapsed_time, camera.get());

	//**********�X�e�[�W�̍X�V**********//
	StageManager::Instance().update(elapsed_time);

	//**********�Q�[���I�u�W�F�N�g�X�V**********//
	EffectManager::instance().update(elapsed_time);
	EffectManager::instance().update_transform();

	if (gamepad.get_button_down() & GamePad::BTN_B)
	{
		//auto game_object = GameObjectManager::instance().create();
		//game_object->set_name("test");
		//game_object->add_component<Missile>();
		//game_object->get_component<Transform>()->set_position(player->get_position());
	}

	//�G�t�F�N�g�X�V
#if _DEBUG
	test_mesh_effect->set_init_life_duration(5);
	//test_mesh_effect->update(graphics,elapsed_time);
	test_mesh_effect->set_is_loop(true);
	test_mesh_effect->get_material().reload();
	//test_emitter->update(elapsed_time);
	//test_meteore->update(graphics, elapsed_time);
#endif
	field_spark_particle->update(graphics.get_dc().Get(), elapsed_time, player->get_position());

}

//==============================================================
// 
//�`��
// 
//==============================================================
void SceneGame::render(float elapsed_time)
{
	StageManager& stageManager = StageManager::Instance();
	Graphics& graphics = Graphics::instance();
	//***************************************************************//
	///						�f�B�t�@�\�h�����_�����O				///
	//***************************************************************//
	{
		deferred->active();
		// �`��X�e�[�g�ݒ�
		graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::SOLID_COUNTERCLOCKWISE);
		graphics.shader_activate(SHADER_TYPE::PBR, RENDER_TYPE::Deferred);

		stageManager.render(elapsed_time, camera.get());
		//�v���C���[�`��
		player->render_d(elapsed_time, camera.get());


		//�����Ŋe�탉�C�e�B���O�i�����A���s���A�_�����j
		deferred->deactive();

		//�����_�[�^�[�Q�b�g��߂�
		graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ADD, ST_RASTERIZER::CULL_NONE);
		camera->get_post_effect()->begin(graphics.get_dc().Get());
		deferred->render();
	}
	//***************************************************************//
	///						�t�H���[�h�����_�����O					///
	//***************************************************************//

	graphics.shader_activate(Graphics::SHADER_TYPES::LAMBERT, RENDER_TYPE::Forward);
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	//�O�㏈���𐳂����s�����߃f�B�t�@�\�h�̐[�x�X�e���V���r���[���g�p����
	//�|�X�g�G�t�F�N�g�ɂ�背���_�[�^�[�Q�b�g�r���[���ς���Ă���̂őΉ�������
	ID3D11RenderTargetView* render_target_views{};
	graphics.get_dc()->OMGetRenderTargets(1, &render_target_views, nullptr);
	graphics.get_dc()->OMSetRenderTargets(1, &render_target_views, deferred->get_dsv());
	//�v���C���[�i�t�H���[�h�j
	player->render_f(elapsed_time, camera.get());
	EffectManager::instance().render(camera.get());
	boss->render_f(elapsed_time, camera.get());

	field_spark_particle->render(graphics.get_dc().Get());


	//�e�X�g�p
#if _DEBUG
	//test_emitter->debug_gui("emitter_test");
	//test_emitter->render(*camera);
#endif
	{
		//�e�X�g�G�t�F�N�g�X�V
		graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::SOLID_ONESIDE);

		//***************************************************************//
		///						�|�X�g�G�t�F�N�g  				        ///
		//***************************************************************//
		graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
		camera->get_post_effect()->end(graphics.get_dc().Get());
		camera->get_post_effect()->blit();

		//***************************************************************//
		///						        UI  				            ///
		//***************************************************************//
		player->render_ui(elapsed_time);
		//camera->debug_gui();

		////�`���[�g���A���`��
		tutorial->render(graphics.get_dc().Get());

		////***************************************************************//
		/////						 �f�o�b�Oprimitive		            ///
		////***************************************************************//
		graphics.get_dc()->OMGetRenderTargets(1, &render_target_views, nullptr);
		graphics.get_dc()->OMSetRenderTargets(1, &render_target_views, deferred->get_dsv());
		graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::WIREFRAME_CULL_BACK);
		debug_figure->render_all_figures(graphics.get_dc().Get());


#if USE_IMGUI
		camera->debug_gui();

		imgui_menu_bar("emitter", "effect", display_imgui);
		if (display_imgui)
		{
			if (ImGui::Button("back_title"))
			{
				//�V�[�����Z�b�g
				SceneManager::instance().change_scene(new SceneLoading(new SceneTitle()));
				return;
			};
			if (ImGui::Button("effect_play"))
			{
				test_effect->play(player->get_position());
			};

			if (ImGui::Button("effect_stop"))
			{
				test_effect->stop();
			};

			test_effect->debug_gui("1");
		}
#endif
	}
}

void SceneGame::clear_update(float elapsedTime)
{
}

void SceneGame::debug_gui()
{

}

void SceneGame::scene_reset()
{
	player->initialize();
}
