#pragma once
#include <functional>
#include "primitive.h"
#include <cereal/cereal.hpp>
#include "camera.h"
struct AttackParam
{
	bool is_attack;//UŒ‚’†‚©‚Ç‚¤‚©
	int power;//UŒ‚—Í
	float invinsible_time;//UŒ‚‘ÎÛ‚É‰Û‚·–³“GŠÔ
	Camera::CameraShakeParam camera_shake;

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

//ƒ_ƒ[ƒW‚ğó‚¯‚½‚Ì‚Ğ‚é‚İ•û
enum class WINCE_TYPE
{
	NONE,//‹¯‚İ‚È‚µ
	SMALL,//¬‚³‚­‚Ğ‚é‚Ş
	BIG,//‘å‚«‚­‚«”ò‚Î‚³‚ê‚é
};
typedef std::function<bool(int, float, WINCE_TYPE)> AddDamageFunc;