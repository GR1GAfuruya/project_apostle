#include "scene_loading.h"
#include "scene_game.h"
#include "scene_manager.h"

#include<thread>


void SceneLoading::initialize(Graphics& graphics)
{
	//�X�v���C�g������
	sprite = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprite\\fire_mask.png",1);

	//�X���b�h�J�n
	std::thread thread(LoadingThread, this, &graphics);

	//�X���b�h�̊Ǘ������
	thread.detach();
}

void SceneLoading::finalize()
{
	sprite.reset();
}

void SceneLoading::update(float elapsedTime, Graphics& graphics)
{
	constexpr float speed = 180;
	angle += speed * elapsedTime;

	//���̃V�[���̏���������������V�[����؂�ւ���

	if (next_scene->is_ready())
	{
		SceneManager::instance().change_scene(graphics, next_scene);
		next_scene = nullptr;
	}
}

void SceneLoading::render(float elapsedTime, Graphics& graphics)
{
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_OFF_ZW_OFF, ST_BLEND::ADD, ST_RASTERIZER::CULL_NONE);
	sprite->begin(graphics.get_dc().Get());
	sprite->render(graphics.get_dc().Get(), { 100, 100 }, { 1, 1 });
	sprite->end(graphics.get_dc().Get());
}

void SceneLoading::LoadingThread(SceneLoading* scene, Graphics* graphics)
{
	//���̃V�[���̏��������s��
	scene->next_scene->initialize(*graphics);
	//���̃V�[���̏��������ݒ�
	scene->next_scene->set_ready(true);
}
