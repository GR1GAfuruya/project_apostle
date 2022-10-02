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
	// �X�e�[�W������
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
	std::shared_ptr<DirectionalLight> d = make_shared<DirectionalLight>(graphics, DirectX::XMFLOAT3(1, -1, 1), 0.7f, 0.3f, 0.2f);
	light_manager->register_light(d);
}

void SceneGame::finalize()
{
	StageManager::Instance().Clear();
}

void SceneGame::update(float elapsed_time, Graphics& graphics)
{
	StageManager& stageManager = StageManager::Instance();
	//�J�����̍X�V
	camera->update(elapsed_time, stage.get());
	camera->calc_view_projection(graphics, elapsed_time);
	camera->set_trakking_target(player.get()->get_gazing_point());

	//�v���C���[�̍X�V
	player->update(graphics, elapsed_time, camera.get(), stage.get());

	player->calc_collision_vs_enemy(boss->boss_collision.position, boss->boss_collision.radius, boss->boss_collision.height);
	
	player->calc_attack_vs_enemy(boss->boss_collision.position, boss->boss_collision.position_end, boss->boss_collision.height / 2, boss->damaged_function);


	//�{�X�̍X�V
	boss->update(graphics, elapsed_time, stage.get());
	
	//�{�X�̍U���Ώۂ�ݒ�
	boss->set_location_of_attack_target(player->get_position());
	boss->calc_attack_vs_player(player->collider.start, player->collider.end, player->collider.radius, player->damaged_function);


	stageManager.update(elapsed_time);

	//particles->update(graphics,elapsed_time);
	Mouse& mouse = Device::instance().get_mouse();

	field_spark_particle->update(graphics.get_dc().Get(), elapsed_time, player->get_position());

	/*boss->calc_attack_vs_player(player->get_position(), { player->get_position().x,player->get_position().y + 5.0f,player->get_position().z }
	, player->get_radius(), player->damaged_function);*/

	//�V�[�����Z�b�g�i���u���j
	if (mouse.get_button() & Mouse::BTN_ENTER)
	{
		scene_reset();
	}
}

void SceneGame::render(float elapsed_time, Graphics& graphics)
{
	///////////////////////////////////////////////////////////////////
	///						�f�B�t�@�\�h�����_�����O				///
	//////////////////////////////////////////////////////////////////
	StageManager& stageManager = StageManager::Instance();
	deferred->active(graphics);
	// �`��X�e�[�g�ݒ�
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::SOLID_COUNTERCLOCKWISE);
	graphics.shader_activate(SHADER_TYPE::PBR,RENDER_TYPE::Deferred);
	player->render_d(graphics,elapsed_time,camera.get());
	boss->render_d(graphics,elapsed_time);
	
	stageManager.render(elapsed_time, graphics);
	//�����Ŋe�탉�C�e�B���O�i�����A���s���A�_�����j
	deferred->deactive(graphics,*light_manager);
	//�����_�[�^�[�Q�b�g��߂�
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ADD, ST_RASTERIZER::CULL_NONE);
	post_effect->begin(graphics.get_dc().Get());
	deferred->render(graphics);

	///////////////////////////////////////////////////////////////////
	///						�t�H���[�h�����_�����O					///
	//////////////////////////////////////////////////////////////////
	graphics.shader_activate(Graphics::SHADER_TYPES::LAMBERT, RENDER_TYPE::Forward);
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	//�O�㏈���𐳂����s�����߃f�B�t�@�\�h�̐[�x�X�e���V���r���[���g�p����
	//�|�X�g�G�t�F�N�g�ɂ�背���_�[�^�[�Q�b�g�r���[���ς���Ă���̂őΉ�������
	ID3D11RenderTargetView* render_target_views{};
	graphics.get_dc()->OMGetRenderTargets(1, &render_target_views, nullptr);
	graphics.get_dc()->OMSetRenderTargets(1, &render_target_views,	deferred->get_dsv());
	//�X�J�C�{�b�N�X
	skybox->render(graphics);
	//�{�X�i�t�H���[�h�j
	boss->render_f(graphics, elapsed_time);
	//�v���C���[�i�t�H���[�h�j
	player->render_f(graphics, elapsed_time, camera.get());
	//�X�e�[�W��ɕ����Ή�
	field_spark_particle->render(graphics.get_dc().Get());
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	//�|�X�g�G�t�F�N�g�p�̃����_�[�����܂�
	post_effect->end(graphics.get_dc().Get());

	post_effect->blit(graphics);

	//UI
	operation_ui->begin(graphics.get_dc().Get());
	operation_ui->render(graphics.get_dc().Get(), { 0, 10 }, { 2, 2 });
	operation_ui->end(graphics.get_dc().Get());	debug_gui();

	player->render_ui(graphics,elapsed_time);
	debug_gui();

	//�f�o�b�O�����_�[
	graphics.get_dc()->OMGetRenderTargets(1, &render_target_views, nullptr);
	graphics.get_dc()->OMSetRenderTargets(1, &render_target_views, deferred->get_dsv());
	debug_figure->render_all_figures(graphics.get_dc().Get());
	
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
