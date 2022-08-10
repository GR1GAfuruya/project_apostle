#include "scene_manager.h"
SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
	//シーンの削除
	clear();
}

void SceneManager::initialize(Graphics& graphics)
{
	if (current_scene != nullptr)
	{
		current_scene->initialize(graphics);
	}
}

void SceneManager::update(float elapsed_time, Graphics& graphics)
{
	if (current_scene != nullptr)
	{
		current_scene->update(elapsed_time, graphics);
	}
}

void SceneManager::render(float elapsed_time, Graphics& graphics)
{
	if(current_scene != nullptr)
	{
		current_scene->render(elapsed_time, graphics);
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

void SceneManager::change_scene(Graphics& graphics,Scene* scene)
{
	//古いシーンの終了処理
	clear();
	current_scene = scene;
	//シーン初期化処理
	if (!current_scene->is_ready()) current_scene->initialize(graphics);
}
