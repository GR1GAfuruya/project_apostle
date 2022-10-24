#pragma once
#include <functional>
#include "primitive.h"

struct AttackParam
{
	Capsule collision;
	bool is_attack;//�U�������ǂ���
	int power;//�U����
	float invinsible_time;//�U���Ώۂɉۂ����G����
};

//�_���[�W���󂯂����̂Ђ�ݕ�
enum class WINCE_TYPE
{
	NONE,//���݂Ȃ�
	SMALL,//�������Ђ��
	BIG,//�傫��������΂����
};
typedef std::function<void(int, float, WINCE_TYPE)> AddDamageFunc;