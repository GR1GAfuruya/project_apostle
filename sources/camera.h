#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "post_effects.h"
#include "constant.h"
#include "graphics.h"
#include "stage.h"
#include "user.h"
#include "noise.h"

#include <cereal/cereal.hpp>
class Camera
{
private:
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
public:
    //------カメラシェイク-------//
    struct CameraShakeParam
    {
        float max_x_shake = 0.0f;//横揺れ最大値　※入力はDegree値で
        float max_y_shake = 0.0f;//縦揺れ最大値　※入力はDegree値で
        float time = 0.0f;//揺れる時間
        float shake_smoothness = 1.0f;//揺れ方の滑らかさ

        // シリアライズ
        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(
                cereal::make_nvp("max_x_shake", max_x_shake),
                cereal::make_nvp("max_y_shake", max_y_shake),
                cereal::make_nvp("time", time),
                cereal::make_nvp("shake_smoothness", shake_smoothness)
            );
        }
    };

    struct HitStopParam
    {
        float time = 0.0f;//止める時間
        float stopping_strength = 5.0f;//止める強度（完全に0にしてしまうとバグるため）
        // シリアライズ
        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(
                cereal::make_nvp("time", time),
                cereal::make_nvp("stopping_strength", stopping_strength)
            );
        }
    };

public:
    //--------<constructor/destructor>--------//
    Camera(Graphics& graphics, const char* post_effect_file_path);
    ~Camera() = default;

    //--------< 関数 >--------//
    void update(float elapsed_time);
    //対象を追従する
    void update_with_tracking(float elapsed_time);
    //ロックオン対象にカメラを向ける
    void update_with_lock_on(float elapsed_time);
    //コントローラーのスティックで操作
    void control_by_game_pad_stick(float elapsed_time);
    //void move_viewing_angle(bool is_move, float elapsed_time){};
    void calc_view_projection(Graphics& graphics, float elapsed_time);
    void debug_gui();
    //ヒットストップ関数
    float hit_stop_update(float elapsed_time);
    //--------<getter/setter>--------//
    // 対象との距離
    void set_range(float r) { range = r; }
    // 見る対象
    void set_lock_on_target(const DirectX::XMFLOAT3& t) { lock_on_target = t; }
    const DirectX::XMFLOAT3& get_lock_on_target() const { return lock_on_target; }
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
    const DirectX::XMFLOAT3& get_forward()const { return Math::get_posture_forward(orientation); }
    //右方向取得
    const DirectX::XMFLOAT3& get_right()const { return Math::get_posture_right(orientation); }
    //ターゲットが移動しているかどうか
    void set_is_move(bool m) { this->is_move = m; }
    //視野角取得
    const float& get_cape_vision()const { return cape_vision; }
    //ロックオン
    const bool get_lock_on() const { return lock_on; }
    void set_lock_on()  {  lock_on = !lock_on; }

    const  DirectX::XMFLOAT4& get_light_color()const { return light_color; }

    std::shared_ptr<PostEffects> get_post_effect() { return post_effect; }
    //PostEffects post_effect;

    //カメラシェイク
    void set_camera_shake(CameraShakeParam param);

    void set_hit_stop(HitStopParam param);
private:
    void calc_free_target();

    void camera_shake_update(float elapsed_time);
    //--------< 関数ポインタ >--------//
    typedef void (Camera::* p_Update)(float elapsed_time);
    p_Update p_update = &Camera::update_with_tracking;
    //--------< 関数 >--------//
    //

    //--------< 変数 >--------//
    std::unique_ptr<Constants<SCENE_CONSTANTS>> scene_constant_buffer{};

    ////注視点からの距離
    float range;
    DirectX::XMFLOAT3 eye; //視点
    DirectX::XMFLOAT3 trakking_target;//注視点
    DirectX::XMFLOAT3 lock_on_target;//注視点
    DirectX::XMFLOAT3 angle;
    DirectX::XMFLOAT4 orientation = { 0,0,0,1 };
    DirectX::XMFLOAT4 standard_orientation = { 0,0,0,1 };

    float lock_on_rate = 6.0f;
    float sensitivity_rate = 0.7f;
    bool is_move;
    float attend_rate; // 減衰比率
    float cape_vision = 60.0f;//視野角
    float roll_speed = 90;//回転速度

    //垂直遅延
    float vertical_rotation_degree = 0;
    //平行遅延
    float horizon_rotation_degree = 0;

    DirectX::XMFLOAT4 light_color = { 1.0f,1.0f, 1.0f,1.0f };
    DirectX::XMFLOAT4 light_direction{ 1.0f,1.0f, 1.0f,1.0f };
    //上下の向ける角度制限
    float max_angle_x;
    float min_angle_x;
    //マウス操作
    bool is_mouse_operation;
    //view
    DirectX::XMFLOAT4X4 view;
    //projection
    DirectX::XMFLOAT4X4 projection;
    //view_projection
    DirectX::XMFLOAT4X4 view_projection;
    //デバッグGUI表示
    bool display_camera_imgui = false;
    bool camera_operate_stop;


    //ロックオンフラグ
    bool lock_on = false;
    float lock_on_angle;//ロックオン対象とカメラ正面ベクトルとの角度（カメラシェイクの仕様をカバーするため）
    //------カメラシェイク-------//
    bool is_camera_shake = false;//カメラシェイク中
    CameraShakeParam camera_shake_param;
    //------ヒットストップ-------//
    bool is_hit_stop = false;//ヒットストップ中
    HitStopParam hit_stop_param;

    //------ポストエフェクト-------//
    std::shared_ptr<PostEffects> post_effect;
};
