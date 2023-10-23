#include "scene_manager.h"
SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
	//シーンの削除
	clear();
}

void SceneManager::initialize()
{
	if (current_scene != nullptr)
	{
		current_scene->initialize();
	}
}

void SceneManager::update(float elapsed_time)
{
	if (current_scene != nullptr)
	{
		current_scene->update(elapsed_time);
	}
}

void SceneManager::render(float elapsed_time)
{
	if (current_scene != nullptr)
	{
		current_scene->render(elapsed_time);
	}
}

void SceneManager::clear()
{
	if (current_scene != nullptr)
	{
		current_scene->finalize();
		delete current_scene;
		current_scene = nullptr;
	}
}

void SceneManager::change_scene(Scene* scene)
{
	//古いシーンの終了処理
	clear();
	current_scene = scene;
	//シーン初期化処理
	if (!current_scene->is_ready()) current_scene->initialize();
}
