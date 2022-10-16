#pragma once
#include "skeletal_mesh.h"
#include "Stage.h"
//�X�e�[�W
class StageMain : public Stage
{
public:
	StageMain(Graphics& graphics);
	~StageMain() override;

	//�C���X�^���X�擾
	static StageMain& Instance();

	//�X�V����
	void update(float elapsedTime)override;

	//�`�揈��
	void render(float elapsed_time, Graphics& graphics)override;

	// ���C�L���X�g
	bool ray_cast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

private:
	std::unique_ptr<SkeletalMesh>  model = nullptr;
	std::unique_ptr<SkeletalMesh>  model_collision = nullptr;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 angle{};
	DirectX::XMFLOAT3 position{};
};
