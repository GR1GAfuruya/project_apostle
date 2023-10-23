#pragma once
#include "scene.h"
class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	static SceneManager& instance()
	{
		static SceneManager instance;
		return instance;
	}

	void initialize();
	void update(float elapsed_time);
	void render(float elapsed_time);

	//シーンクリア
	void clear();

	void change_scene(Scene* scene);

private:
	//現在のシーン
	Scene* current_scene = nullptr;
};
