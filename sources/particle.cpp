#include "particle.h"

#include "shader.h"
#include "SimpleMath.h"
#include "texture.h"
#include "imgui_include.h"
#include "Operators.h"
#include "noise.h"
#include "user.h"
//==============================================================
// 
//コンストラクタ
// 
//==============================================================
Particles::Particles(Param init_param)
{
	is_active = true;

	//パラメーター初期化
	param = init_param;
	param.transition.position.x = param.transition.position.x + Noise::instance().random_fixed_range(param.transition.random_position.x);
	param.transition.position.y = param.transition.position.y + Noise::instance().random_fixed_range(param.transition.random_position.y);
	param.transition.position.z = param.transition.position.z + Noise::instance().random_fixed_range(param.transition.random_position.z);

	param.transition.velocity.x = param.transition.velocity.x + Noise::instance().random_fixed_range(param.transition.random_velocity.x);
	param.transition.velocity.y = param.transition.velocity.y + Noise::instance().random_fixed_range(param.transition.random_velocity.y);
	param.transition.velocity.z = param.transition.velocity.z + Noise::instance().random_fixed_range(param.transition.random_velocity.z);

	param.transition.acceleration.x = param.transition.acceleration.x + Noise::instance().random_fixed_range(param.transition.random_acceleration.x);
	param.transition.acceleration.y = param.transition.acceleration.y + Noise::instance().random_fixed_range(param.transition.random_acceleration.y);
	param.transition.acceleration.z = param.transition.acceleration.z + Noise::instance().random_fixed_range(param.transition.random_acceleration.z);


}
//==============================================================
// 
//デストラクタ
// 
//==============================================================
Particles::~Particles()
{

}
//==============================================================
// 
//初期化
// 
//==============================================================
void Particles::initialize()
{
}


//==============================================================
// 
//更新
// 
//==============================================================
void Particles::update(float elapsed_time)
{
	//位置更新
	position_update(elapsed_time);
	//姿勢更新
	orientation_update(elapsed_time);

	//寿命更新
	life_update(elapsed_time);
}

void Particles::look_at_camera(Camera& camera)
{
	DirectX::XMFLOAT4 ori = camera.get_orientation();
	DirectX::XMFLOAT3 Axis;
	Axis = Math::get_posture_up(ori);
	ori = Math::rot_quaternion(ori, Axis, DirectX::XMConvertToRadians(180.0f));

	param.orientation = ori;

	Axis = Math::get_posture_forward(param.orientation);
	param.orientation = Math::rot_quaternion(param.orientation, Axis, DirectX::XMConvertToRadians(param.rotate.rotation.z));
}

//==============================================================
// 
//位置更新
// 
//==============================================================
void Particles::position_update(float elapsed_time)
{
	param.transition.velocity += param.transition.acceleration * elapsed_time;

	param.transition.position += param.transition.velocity * elapsed_time;
}

//==============================================================
// 
//姿勢更新
// 
//==============================================================
void Particles::orientation_update(float elapsed_time)
{
	DirectX::XMFLOAT3 Axis;
	Axis = Math::get_posture_right(param.orientation);
	param.orientation = Math::rot_quaternion(param.orientation, Axis, param.rotate.rotation.x);

	Axis = Math::get_posture_up(param.orientation);
	param.orientation = Math::rot_quaternion(param.orientation, Axis, param.rotate.rotation.y);

	Axis = Math::get_posture_forward(param.orientation);
	param.orientation = Math::rot_quaternion(param.orientation, Axis, param.rotate.rotation.z);
}


//==============================================================
// 
//寿命更新
// 
//==============================================================
void Particles::life_update(float elapsed_time)
{
	age += elapsed_time;
	if (age > param.life_time)
	{
		is_active = false;
	}

}

