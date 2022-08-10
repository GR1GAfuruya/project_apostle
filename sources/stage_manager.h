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
	void render(float elapsed_time, Graphics& graphics);
	//ステージ登録
	void Register(Stage* stage);
	//ステージ全削除
	void Clear();
	//レイキャスト
	bool Raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) ;

private:
	std::vector<Stage*> stages;
};