#pragma once
#include "scene.h"
#include "sprite_batch.h"
#include <memory>

class SceneTitle :public Scene
{
public:
	SceneTitle(Graphics& graphics);
	~SceneTitle()override {}

	void initialize(Graphics& graphics) override;
	void finalize() override;
	void update(float elapsedTime, Graphics& graphics) override;
	void render(float elapsedTime,Graphics& graphics) override;

private:
	std::unique_ptr<SpriteBatch> sprite = nullptr;
};
