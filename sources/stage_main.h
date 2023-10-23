#pragma once
#include "skeletal_mesh.h"
#include "Stage.h"
//�X�e�[�W
class StageMain : public Stage
{
public:
	StageMain();
	~StageMain() override;

	//�C���X�^���X�擾
	static StageMain& Instance();

	//�X�V����
	void update(float elapsedTime)override;

	//�`�揈��
	void render(float elapsed_time, Camera* camera)override;
	void  shadow_render(float elapsed_time)override;
	// ���C�L���X�g
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
