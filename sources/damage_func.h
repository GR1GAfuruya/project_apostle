#pragma once
#include <functional>
#include "primitive.h"
#include <cereal/cereal.hpp>
struct AttackParam
{
	Capsule collision;
	bool is_attack;//�U�������ǂ���
	int power;//�U����
	float invinsible_time;//�U���Ώۂɉۂ����G����

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			cereal::make_nvp("power", power),
			cereal::make_nvp("invinsible_time", invinsible_time)
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