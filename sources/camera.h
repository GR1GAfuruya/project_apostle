#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include "constant.h"
#include "graphics.h"
#include "stage.h"
class Camera
{
public:
    //--------<constructor/destructor>--------//
    Camera(Graphics& graphics);
    ~Camera() = default;
public:
    //--------< 関数 >--------//
    void update(float elapsed_time);
    void update_with_tracking(float elapsed_time);
    void update_with_euler_angles(float elapsed_time);
    void update_with_quaternion(float elapsed_time);
    //void move_viewing_angle(bool is_move, float elapsed_time){};
    void calc_view_projection(Graphics& graphics, float elapsed_time);
    void debug_gui();
    //--------<getter/setter>--------//
    // 対象との距離
    void set_range(float r) { range = r; }
    // 見る対象
    void set_target(const DirectX::XMFLOAT3& t) { target = t; }
    const DirectX::XMFLOAT3& get_target() const { return target; }
    // 追尾する対象
    void set_trakking_target(const DirectX::XMFLOAT3& t) { trakking_target = t; }
    const DirectX::XMFLOAT3& get_trakking_target() const { return trakking_target; }
    // 角度
    void set_angle(const DirectX::XMFLOAT3& a) { angle = a; }
    void set_camera_operate_stop(bool s) { camera_operate_stop = s; }
    //視野角設定
    void set_cape_vision(const float& cape) { this->cape_vision = cape; }
    //遅延時間
    void SetAttendRate(float rate) { attend_rate = rate; }
    //対象との距離
    const float& get_range() const { return range; }
    // 光のさす方向
    const DirectX::XMFLOAT4& get_light_direction() const { return light_direction; }
    // view
    const DirectX::XMFLOAT4X4& get_view() const { return view; }
    // projection
    const DirectX::XMFLOAT4X4& get_projection() const { return projection; }
    //視点取得
    const DirectX::XMFLOAT3& get_eye()const { return eye; }
    //前方向取得
    const DirectX::XMFLOAT3& get_forward()const { return forward; }
    //右方向取得
    const DirectX::XMFLOAT3& get_right()const { return right; }
    //ターゲットが移動しているかどうか
    void set_is_move(bool m) { this->is_move = m; }
    //視野角取得
    const float& get_cape_vision()const { return cape_vision; }

    const  DirectX::XMFLOAT4& get_light_color()const { return light_color; }
    

private:
    void calc_free_target();
    //--------< 関数ポインタ >--------//
    typedef void (Camera::* p_Update)(float elapsed_time);
    p_Update p_update = &Camera::update_with_tracking;
    //--------< 関数 >--------//
    //
    //--------< 定数/構造体 >--------//
    struct SCENE_CONSTANTS
    {
        DirectX::XMFLOAT4X4 view;                //ビュー行列
        DirectX::XMFLOAT4X4 projection;          //プロジェクション行列
        DirectX::XMFLOAT4X4 view_projection;     //ビュー・プロジェクション変換行列
        DirectX::XMFLOAT4 light_color;       //ライトの色
        DirectX::XMFLOAT4 light_direction;       //ライトの向き
        DirectX::XMFLOAT4 camera_position;
        DirectX::XMFLOAT4 avatar_position;
        DirectX::XMFLOAT4 avatar_direction;
        DirectX::XMFLOAT2 resolution;
        float time;
        float delta_time;
    };
    //--------< 変数 >--------//
    std::unique_ptr<Constants<SCENE_CONSTANTS>> scene_constant_buffer{};


    float range;
    DirectX::XMFLOAT3 eye; //視点
    DirectX::XMFLOAT3 trakking_target;//注視点
    DirectX::XMFLOAT3 target;//注視点
    DirectX::XMFLOAT3 angle;
    DirectX::XMFLOAT4 orientation;
    //カメラのベクトル
    DirectX::XMFLOAT3 forward;
    DirectX::XMFLOAT3 right;

    bool is_move;
    float attend_rate; // 減衰比率
    float cape_vision = 60.0f;//視野角
    float roll_speed = 90;

    float vertical_rotation_degree = 0;
    float horizon_rotation_degree = 0;
    DirectX::XMFLOAT4 light_color = { 1.0f,1.0f, 1.0f,1.0f };
    DirectX::XMFLOAT4 light_direction{ 1.0f,1.0f, 1.0f,1.0f };
    float max_angle_x;
    float min_angle_x;
    bool is_mouse_operation;
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;
    DirectX::XMFLOAT4X4 view_projection;
    bool display_camera_imgui = false;
    bool camera_operate_stop;

};
