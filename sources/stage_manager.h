#pragma once
#include <vector>
#include "Stage.h"

//ステージマネージャ
class StageManager
{
private:
	StageManager(){}
	~StageManager(){}

public:
	static StageManager& Instance()
	{
		static StageManager instance;
		return instance;
	}

	//更新処理
	void update(float elapsedTime);
	//描画処理
	void render(Graphics& graphics, float elapsed_time, Camera* camera);
	void shadow_render(Graphics& graphics, float elapsed_time);
	//ステージ登録
	void Register(Stage* stage);
	//ステージ全削除
	void Clear();
	//レイキャスト
	bool ray_cast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) ;

private:
	std::vector<Stage*> stages;
};