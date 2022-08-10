#include "move_behavior.h"

#include "user.h"


void MoveBehavior::MovingProcess(float vx, float vz, float speed)
{
    //移動方向ベクトルを設定
    move_vec_x = vx;
    move_vec_z = vz;
    //最大速度設定
    max_move_speed = speed;
}

void MoveBehavior::Turn(float elapsed_time, DirectX::XMFLOAT3 move_velocity, float speed, DirectX::XMFLOAT3 position, DirectX::XMFLOAT4& orientation)
{
    using namespace DirectX;
    speed *= elapsed_time;
    DirectX::XMVECTOR orientation_vec{ DirectX::XMLoadFloat4(&orientation) };

    float length = sqrtf((move_velocity.x * move_velocity.x) + (move_velocity.z * move_velocity.z));
    if (length < 0.1f) return;

    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMVECTOR forward, up;
    m4x4 = QuaternionConvertXMFLOAT4X4(orientation);
    up = { 0, 1, 0 };
    //up = { m4x4._21, m4x4._22, m4x4._23 };
    forward = { m4x4._31, m4x4._32, m4x4._33 };
    XMFLOAT3 front{};
    XMStoreFloat3(&front, forward);
    front.y = 0;
    XMFLOAT3 point1 = Math::calc_designated_point(position, Math::Normalize(front), Math::Length(Math::Normalize(front)));
    XMVECTOR point1_vec = XMLoadFloat3(&point1);
    XMVECTOR d = point1_vec - XMLoadFloat3(&position);

    move_velocity.y = 0;
    XMFLOAT3 point2 = Math::calc_designated_point(position, Math::Normalize(move_velocity), Math::Length(Math::Normalize(move_velocity)));
    XMVECTOR point2_vec = XMLoadFloat3(&point2);
    XMVECTOR d2 = point2_vec - XMLoadFloat3(&position);

    float angle;
    XMVECTOR dot{ XMVector3Dot(d,d2) };
    XMStoreFloat(&angle, dot);
    angle = acosf(angle);

    if (fabs(angle) > XMConvertToRadians(10.0f))
    {
        XMVECTOR q;
        float cross{ (move_velocity.x * front.z) - (move_velocity.z * front.x) };
        if (cross > 0)
        {
            q = XMQuaternionRotationAxis(up, angle);
        }
        else
        {
            q = XMQuaternionRotationAxis(up, -angle);
        }
        XMVECTOR Q = XMQuaternionMultiply(orientation_vec, q);
        orientation_vec = XMQuaternionSlerp(orientation_vec, Q, 10.0f * elapsed_time);
    }
    DirectX::XMStoreFloat4(&orientation, orientation_vec);
}