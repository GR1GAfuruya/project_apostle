#include "particle.h"

#include "shader.h"
#include "SimpleMath.h"
#include "texture.h"
#include "imgui_include.h"
#include "Operators.h"
/***************
 未完成
 *************/

Particles::Particles(Graphics& graphics, InitParam init_param)
{
	is_active = true;

	//位置
	position = init_param.position;
	//射出方向
	velocity = init_param.velocity;
	//色
	color = init_param.color;
	//大きさ
	scale = init_param.scale;
	//寿命
	life_time = init_param.life_time;
}

Particles::~Particles()
{

}

void Particles::initialize(Graphics& graphics)
{
}

inline auto rotate(float& x, float& y, float cx, float cy, float cos, float sin)
{
	x -= cx;
	y -= cy;

	float tx{ x }, ty{ y };
	x = cos * tx + -sin * ty;
	y = sin * tx + cos * ty;

	x += cx;
	y += cy;
};

void Particles::update(Graphics& graphics,float elapsed_time)
{
	//位置更新
	position_update(elapsed_time);

	//寿命更新
	life_update(elapsed_time);
}

void Particles::position_update(float elapsed_time)
{
	position += velocity;
}

void Particles::life_update(float elapsed_time)
{
	time += elapsed_time;
	if (time > life_time)
	{
		is_active = false;
	}

}

