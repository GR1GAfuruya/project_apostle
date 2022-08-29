#include "effect_base.h"
#include "user.h"
void EffecttBase::stop()
{
	active = false;
	life_time = 0;
}
void EffecttBase::update(Graphics& graphics, float elapsed_time)
{
}

void EffecttBase::set_rotate_quaternion(DirectX::XMFLOAT3 axis, float ang)
{
	float angle = DirectX::XMConvertToRadians(ang);
	orientation = Math::rot_quaternion(orientation, axis, angle);
}

void EffecttBase::set_rotate_quaternion(AXIS axis, float ang)
{
	float angle = DirectX::XMConvertToRadians(ang);
	DirectX::XMFLOAT3 Axis;
	switch (axis)
	{
	case EffecttBase::AXIS::RIGHT:
		Axis = Math::get_posture_right(orientation);
		orientation = Math::rot_quaternion(orientation, Axis, angle);
		break;
	case EffecttBase::AXIS::UP:
		Axis = Math::get_posture_up(orientation);
		orientation = Math::rot_quaternion(orientation, Axis, angle);
		break;
	case EffecttBase::AXIS::FORWARD:
		Axis = Math::get_posture_forward(orientation);
		orientation = Math::rot_quaternion(orientation, Axis, angle);
		break;
	default:
		break;
	}

}
