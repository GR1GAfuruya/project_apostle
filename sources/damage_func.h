#pragma once
#include <functional>
#include "primitive.h"
typedef std::function<void(int, float)> AddDamageFunc;

struct AttackParam
{
	Capsule collision;
	bool is_attack;//UŒ‚’†‚©‚Ç‚¤‚©
	int power;//UŒ‚—Í
	float invinsible_time;//UŒ‚‘ÎÛ‚É‰Û‚·–³“GŠÔ
};