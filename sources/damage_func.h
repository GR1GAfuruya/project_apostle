#pragma once
#include <functional>
#include "primitive.h"
#include <cereal/cereal.hpp>
struct AttackParam
{
	Capsule collision;
	bool is_attack;//攻撃中かどうか
	int power;//攻撃力
	float invinsible_time;//攻撃対象に課す無敵時間

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			cereal::make_nvp("power", power),
			cereal::make_nvp("invinsible_time", invinsible_time)
		);
	}
};

//ダメージを受けた時のひるみ方
enum class WINCE_TYPE
{
	NONE,//怯みなし
	SMALL,//小さくひるむ
	BIG,//大きく吹き飛ばされる
};
typedef std::function<bool(int, float, WINCE_TYPE)> AddDamageFunc;