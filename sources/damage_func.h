#pragma once
#include <functional>
#include "primitive.h"
typedef std::function<void(int, float)> AddDamageFunc;

struct AttackParam
{
	Capsule collision;
	bool is_attack;//�U�������ǂ���
	int power;//�U����
	float invinsible_time;//�U���Ώۂɉۂ����G����
};