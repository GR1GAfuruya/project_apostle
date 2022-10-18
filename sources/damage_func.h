#pragma once
#include <functional>
#include "primitive.h"

struct AttackParam
{
	Capsule collision;
	bool is_attack;//UŒ‚’†‚©‚Ç‚¤‚©
	int power;//UŒ‚—Í
	float invinsible_time;//UŒ‚‘ÎÛ‚É‰Û‚·–³“GŠÔ
};

//ƒ_ƒ[ƒW‚ğó‚¯‚½‚Ì‚Ğ‚é‚İ•û
enum class WINCE_TYPE
{
	NONE,//‹¯‚İ‚È‚µ
	SMALL,//¬‚³‚­‚Ğ‚é‚Ş
	BIG,//‘å‚«‚­‚«”ò‚Î‚³‚ê‚é
};
typedef std::function<void(int, float, WINCE_TYPE)> AddDamageFunc;