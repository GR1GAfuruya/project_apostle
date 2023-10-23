#pragma once
#include "skeletal_mesh.h"
#include "Stage.h"
//ステージ
class StageMain : public Stage
{
public:
	StageMain();
	~StageMain() override;

	//インスタンス取得
	static StageMain& Instance();

	//更新処理
	void update(float elapsedTime)override;

	//描画処理
	void render(float elapsed_time, Camera* camera)override;
	void  shadow_render(float elapsed_time)override;
	// レイキャスト
	bool ray_cast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

private:
	std::unique_ptr<SkeletalMesh>  model = nullptr;
	std::unique_ptr<SkeletalMesh>  model_collision = nullptr;
	std::unique_ptr<SkeletalMesh>  model_shadow = nullptr;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 angle{};
	DirectX::XMFLOAT3 position{};
	DirectX::XMFLOAT4X4 transform{};
	bool flustm_flag = false;
};
