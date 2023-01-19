#pragma once
#include <vector>
#include "Stage.h"

//�X�e�[�W�}�l�[�W��
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

	//�X�V����
	void update(float elapsedTime);
	//�`�揈��
	void render(Graphics& graphics, float elapsed_time, Camera* camera);
	void shadow_render(Graphics& graphics, float elapsed_time);
	//�X�e�[�W�o�^
	void Register(Stage* stage);
	//�X�e�[�W�S�폜
	void Clear();
	//���C�L���X�g
	bool ray_cast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) ;

private:
	std::vector<Stage*> stages;
};