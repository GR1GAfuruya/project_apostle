#pragma once
#include "mat_fire_distortion.h"
#include "mat_meteore.h"
#include "mat_lightning.h"
class MaterialManager
{
private:
	//�R���X�g���N�^/�f�X�g���N�^
	MaterialManager(){}
	~MaterialManager(){}
public:
	//�����̃}�e���A������
	void create_materials(Graphics& graohics);
	static MaterialManager& instance()
	{
		static MaterialManager instance;
		return instance;
	}

	std::shared_ptr<FireDistortionMat> mat_fire_distortion;
	std::shared_ptr<MeteoreMat> mat_meteore;
	std::shared_ptr<LightningMat> mat_lightning;
};