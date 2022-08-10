#pragma once
#include "skeletal_mesh.h"
#include "Stage.h"
//ステージ
class StageMain : public Stage
{
public:
	StageMain(Graphics& graphics);
	~StageMain() override;

	//インスタンス取得
	static StageMain& Instance();

	//更新処理
	void update(float elapsedTime)override;

	//描画処理
	void render(float elapsed_time, Graphics& graphics)override;

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

private:
	std::unique_ptr<SkeletalMesh>  model = nullptr;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 angle{};
	DirectX::XMFLOAT3 position{};
};
