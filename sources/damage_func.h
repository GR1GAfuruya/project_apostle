#pragma once
#include <functional>
#include "primitive.h"

struct AttackParam
{
	Capsule collision;
	bool is_attack;//攻撃中かどうか
	int power;//攻撃力
	float invinsible_time;//攻撃対象に課す無敵時間
};

//ダメージを受けた時のひるみ方
enum class WINCE_TYPE
{
	NONE,//怯みなし
	SMALL,//小さくひるむ
	BIG,//大きく吹き飛ばされる
};
typedef std::function<void(int, float, WINCE_TYPE)> AddDamageFunc;