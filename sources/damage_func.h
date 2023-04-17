#pragma once
#include <functional>
#include "primitive.h"
#include <cereal/cereal.hpp>
#include "camera.h"
#include "game_pad.h"
struct AttackParam
{
	bool is_attack;//攻撃中かどうか
	int power;//攻撃力
	float invinsible_time;//攻撃対象に課す無敵時間
	Camera::CameraShakeParam camera_shake;//カメラシェイク
	Camera::HitStopParam hit_stop;//ヒットストップ
	GamePad::Viberation hit_viberation;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			cereal::make_nvp("power", power),
			cereal::make_nvp("invinsible_time", invinsible_time),
			cereal::make_nvp("camera_shake", camera_shake),
			cereal::make_nvp("hit_stop", hit_stop),
			cereal::make_nvp("hit_viberation", hit_viberation)
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