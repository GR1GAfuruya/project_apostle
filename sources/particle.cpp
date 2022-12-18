#include "particle.h"

#include "shader.h"
#include "SimpleMath.h"
#include "texture.h"
#include "imgui_include.h"
#include "Operators.h"
/***************
 ������
 *************/

Particles::Particles(Graphics& graphics, InitParam init_param)
{
	is_active = true;

	//�ʒu
	position = init_param.position;
	//�ˏo����
	velocity = init_param.velocity;
	//�F
	color = init_param.color;
	//�傫��
	scale = init_param.scale;
	//����
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
	//�ʒu�X�V
	position_update(elapsed_time);

	//�����X�V
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

