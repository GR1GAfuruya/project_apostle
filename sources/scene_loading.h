#pragma once

#include "sprite_batch.h"
#include "scene.h"
#include <memory>
//���[�f�B���O�V�[��
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene) : next_scene(nextScene) {}
	~SceneLoading() override {}
	void initialize(Graphics& graphics) override;
	void finalize() override;
	void update(float elapsedTime, Graphics& graphics) override;
	void render(float elapsedTime, Graphics& graphics) override;
private:
	//���[�f�B���O�X���b�h
	static void LoadingThread(SceneLoading* scene, Graphics* graphics);

	std::unique_ptr<SpriteBatch> sprite = nullptr;
	float angle = 0.0f;
	Scene* next_scene = nullptr;

};
