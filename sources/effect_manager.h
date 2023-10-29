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

	// 作成
	std::shared_ptr<GameObject> create();

	// 削除
	void remove(std::shared_ptr<GameObject> game_object);

	// 更新
	void update(float elapsed_time);

	// 行列更新
	void update_transform();

	// 描画
	void render(Camera* camera);


private:
	std::vector<std::shared_ptr<GameObject>> start_game_objects;
	std::vector<std::shared_ptr<GameObject>> update_game_objects;
	std::vector<std::shared_ptr<GameObject>> remove_game_objects;
};