#pragma once
#include "game_object.h"

class EffectManager
{
private:
	EffectManager() {};
	~EffectManager() {};
public:
	static EffectManager& instance()
	{
		static EffectManager instance;
		return instance;
	}

	// �쐬
	std::shared_ptr<GameObject> create();

	// �폜
	void remove(std::shared_ptr<GameObject> game_object);

	// �X�V
	void update(float elapsed_time);

	// �s��X�V
	void update_transform();

	// �`��
	void render(Camera* camera);


private:
	std::vector<std::shared_ptr<GameObject>> start_game_objects;
	std::vector<std::shared_ptr<GameObject>> update_game_objects;
	std::vector<std::shared_ptr<GameObject>> remove_game_objects;
};