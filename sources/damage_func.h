#pragma once
#include <functional>
#include "primitive.h"
#include <cereal/cereal.hpp>
#include "camera.h"
struct AttackParam
{
	bool is_attack;//�U�������ǂ���
	int power;//�U����
	float invinsible_time;//�U���Ώۂɉۂ����G����
	Camera::CameraShakeParam camera_shake;//�J�����V�F�C�N

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			cereal::make_nvp("power", power),
			cereal::make_nvp("invinsible_time", invinsible_time),
			cereal::make_nvp("camera_shake", camera_shake)
		);
	}
};

//�_���[�W���󂯂����̂Ђ�ݕ�
enum class WINCE_TYPE
{
	NONE,//���݂Ȃ�
	SMALL,//�������Ђ��
	BIG,//�傫��������΂����
};
typedef std::function<bool(int, float, WINCE_TYPE)> AddDamageFunc;