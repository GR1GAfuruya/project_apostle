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

	void initialize(Graphics& graphics);
	void update(float elapsed_time,Graphics& graphics);
	void render(float elapsed_time, Graphics& graphics);

	//シーンクリア
	void clear();

	void change_scene(Graphics& graphics, Scene* scene);

private:
	//現在のシーン
	Scene* current_scene = nullptr;
};
