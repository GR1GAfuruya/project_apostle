#include "scene_loading.h"
#include "scene_game.h"
#include "scene_manager.h"

#include<thread>


void SceneLoading::initialize()
{

	//�X�v���C�g������
	sprite = std::make_unique<SpriteBatch>(Graphics::instance().get_device().Get(), L".//resources//Sprite//Title//loading2.png", 1);

	//�X���b�h�J�n
	std::thread thread(LoadingThread, this);

	//�X���b�h�̊Ǘ������
	thread.detach();
}

void SceneLoading::finalize()
{
	sprite.reset();
}

void SceneLoading::update(float elapsedTime)
{
	constexpr float speed = 180;
	angle += speed * elapsedTime;

	//���̃V�[���̏���������������V�[����؂�ւ���

	if (next_scene->is_ready())
	{
		SceneManager::instance().change_scene(next_scene);
		next_scene = nullptr;
	}
}

void SceneLoading::render(float elapsedTime)
{
	Graphics& graphics = Graphics::instance();
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_OFF_ZW_OFF, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	sprite->begin(graphics.get_dc().Get());
	sprite->render(graphics.get_dc().Get(), { 0, 0 }, { 1, 1 });
	sprite->end(graphics.get_dc().Get());
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
	//���̃V�[���̏��������s��
	scene->next_scene->initialize();
	//���̃V�[���̏��������ݒ�
	scene->next_scene->set_ready(true);
}
