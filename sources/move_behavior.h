#pragma once
#include "collision.h"
#include "stage.h"
class MoveBehavior
{
public:
    MoveBehavior() {}
    virtual ~MoveBehavior() {}
protected:
    DirectX::XMFLOAT3 velocity{};
    float acceleration{ 1.0f };
    float max_move_speed{ 100.0f };
    float move_vec_x{};
    float move_vec_y{};
    float move_vec_z{};
    float friction{ 0.5f };
    float move_speed{ 30.0f };
    float turn_speed{ DirectX::XMConvertToRadians(720) };
    //レイキャストの変数
    HitResult hit{};
    bool raycast{ false };
    float step_offset_y{ 2.0f };
    float step_offset_z{ 1.0f };
protected:
    //垂直速力更新処理
    virtual  void update_vertical_velocity(float elapsed_flame) {}
    //垂直移動更新処理
    virtual void update_vertical_move(float elapsed_time, DirectX::XMFLOAT3& position, Stage* stage) {}
    //水平速力更新処理
    virtual void update_hrizontal_velocity(float elapsed_time) {}
    //水平移動更新処理
    virtual void update_horizontal_move(float elapsed_time, DirectX::XMFLOAT3& position, Stage* stage) {}
public:
    void MovingProcess(float vx, float vz, float speed);
    virtual void Turn(float elapsed_time, DirectX::XMFLOAT3 move_velocity, float speed, DirectX::XMFLOAT3 position, DirectX::XMFLOAT4& orientation);
    //--------------------------------------------//
};


inline DirectX::XMFLOAT4X4 QuaternionConvertXMFLOAT4X4(DirectX::XMFLOAT4 orientation)
{
    using namespace DirectX;

    XMVECTOR orientation_vec{ XMLoadFloat4(&orientation) };
    DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(orientation_vec);
    DirectX::XMFLOAT4X4 m4x4 = {};
    DirectX::XMStoreFloat4x4(&m4x4, m);

    return m4x4;
}