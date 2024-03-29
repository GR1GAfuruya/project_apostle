﻿#include "framework.h"
#include "camera.h"
#include "imgui_include.h"
#include "Operators.h"
#include "stage_manager.h"
#include "graphics.h"
//==============================================================
// 
//コンストラクタ
// 
//==============================================================
Camera::Camera(const char* post_effect_file_path)
	: range(20.0f)
	, eye(5, 5, 5)
	, angle(DirectX::XMConvertToRadians(0.0f), DirectX::XMConvertToRadians(-180.0f), 0)
	, trakking_target(30.0f, 10, 0)
	, lock_on_target(30.0f, 10, 0)
	, light_direction(0.6f, -1, 0.1f, 0.7f)
	, roll_speed(200)
	, attend_rate(8.0f)
	, max_angle_x(DirectX::XMConvertToRadians(60))
	, min_angle_x(DirectX::XMConvertToRadians(-60))
	, view()
	, projection()
	, is_mouse_operation(false)

{
	HRESULT hr{ S_OK };
	//----定数バッファ----//
	// カメラ関連
	{
		scene_constant_buffer = std::make_unique<Constants<SCENE_CONSTANTS>>(Graphics::instance().get_device().Get());
	}
	// orientationの初期化
	{
		DirectX::XMFLOAT3 n(0, 1, 0); // 軸（正規化）
		constexpr float angle = DirectX::XMConvertToRadians(0); //角度（ラジアン）
		orientation = {
			sinf(angle / 2) * n.x,
			sinf(angle / 2) * n.y,
			sinf(angle / 2) * n.z,
			cosf(angle / 2)
		};

		standard_orientation = {
			sinf(angle / 2) * n.x,
			sinf(angle / 2) * n.y,
			sinf(angle / 2) * n.z,
			cosf(angle / 2)
		};
	}
	// eyeの初期化
	{
		//カメラ回転値を回転行列に変換
		DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		//回転行列の前方向ベクトルを取り出す
		DirectX::XMVECTOR Front = Transform.r[2];
		DirectX::XMFLOAT3 front;
		DirectX::XMStoreFloat3(&front, Front);
		//注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
		eye.x = trakking_target.x - front.x * range;
		eye.y = trakking_target.y - front.y * range;
		eye.z = trakking_target.z - front.z * range;
	}
	post_effect = std::make_shared<PostEffects>(Graphics::instance().get_device().Get(), post_effect_file_path);
}
//==============================================================
// 
//更新
// 
//==============================================================
void Camera::update(float elapsed_time)
{
	using namespace DirectX;
	// 任意のアップデートを実行
	(this->*p_update)(elapsed_time);

	//ロックオン時の挙動
	if (lock_on)
	{
		//ロックオン時のカメラの挙動
		update_with_lock_on(elapsed_time);
	}
	else
	{
		//ロックオンじゃないときは０に（前の値が残ってカメラシェイクの挙動バグる）
		lock_on_angle = 0;
		//ロックオンしていないときのカメラの挙動
		if (!camera_operate_stop)
		{
			control_by_game_pad_stick(elapsed_time);
		}
	}


	//カメラのパラメータ設定(全アップデート共通)
	{
		//世界の上方向
		DirectX::XMFLOAT3 up(0, 1, 0);
		XMVECTOR up_vec = XMLoadFloat3(&up);
		//視線ベクトル
		XMVECTOR eye_vec = XMLoadFloat3(&eye);
		//注視対象
		XMVECTOR focus_vec = XMLoadFloat3(&trakking_target);
		//view行列
		XMMATRIX view_mat = XMMatrixLookAtLH(eye_vec, focus_vec, up_vec);
		XMStoreFloat4x4(&view, view_mat);

		// プロジェクション行列を作成
		float width = static_cast<float>(SCREEN_WIDTH);
		float height = static_cast<float>(SCREEN_HEIGHT);
		float aspect_ratio{ width / height };
		static DirectX::XMFLOAT2 near_far = { 0.1f, 5000.0f };

#ifdef USE_IMGUI
		if (display_camera_imgui)
		{
			ImGui::Begin("main_camera");
			ImGui::DragFloat2("near_far", &near_far.x, 0.1f, 0.1f, 2000.0f);
			ImGui::End();
		}
#endif // USE_IMGUI
		XMMATRIX projection_mat = XMMatrixPerspectiveFovLH(XMConvertToRadians(cape_vision), aspect_ratio, near_far.x, near_far.y); // P
		XMStoreFloat4x4(&projection, projection_mat);
	}

}


//==============================================================
// 
//追従更新
// 
//==============================================================
void Camera::update_with_tracking(float elapsed_time)
{

	//Y軸の回転値を-3.14~3.14に収まるようにする
	if (angle.y < -DirectX::XM_PI) { angle.y += DirectX::XM_2PI; }
	if (angle.y > DirectX::XM_PI) { angle.y -= DirectX::XM_2PI; }

	//カメラシェイク
	camera_shake_update(elapsed_time);

	// カメラ回転値を回転行列に変換
	// XMVECTORクラスへ変換
	DirectX::XMFLOAT3 forward = Math::get_posture_forward(orientation);

	// レイキャスト(ターゲットと壁)
	DirectX::XMFLOAT3 ray_target = trakking_target + DirectX::XMFLOAT3{ 0,-0.5,0 };//めり込まないよう少し下に下げる
	DirectX::XMFLOAT3 start = ray_target;
	DirectX::XMFLOAT3 end = ray_target - forward * DirectX::XMFLOAT3(range, range, range);
	HitResult hit;
	StageManager::Instance().ray_cast(start, end, hit);

	hit.distance = (std::max)(hit.distance, 0.5f);
	hit.distance = (std::min)(hit.distance, range);


	//注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	DirectX::XMFLOAT3 pos;
	pos.x = trakking_target.x - forward.x * hit.distance;
	pos.y = trakking_target.y - forward.y * hit.distance;
	pos.z = trakking_target.z - forward.z * hit.distance;
	eye = Math::lerp(eye, pos, attend_rate * elapsed_time);
}
//==============================================================
// 
//ロックオン更新
// 
//==============================================================
void Camera::update_with_lock_on(float elapsed_time)
{
	// XMVECTORクラスへ変換
	// カメラの現在位置から、目標座標への方向を求める
	DirectX::XMFLOAT3 dir = lock_on_target - eye;
	DirectX::XMVECTOR TargeVecNorm = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&dir));
	DirectX::XMVECTOR Forward = Math::get_posture_forward_vec(orientation);
	DirectX::XMVECTOR Up = { 0.0f,1.0f,0.0f };
	DirectX::XMVECTOR orientationVec = DirectX::XMLoadFloat4(&orientation);

	DirectX::XMVECTOR dot = DirectX::XMVector3Dot(Forward, TargeVecNorm);
	DirectX::XMStoreFloat(&lock_on_angle, dot);
	lock_on_angle = acosf(lock_on_angle);

	DirectX::XMFLOAT3 forward{};//forwardの値をfloat3に
	DirectX::XMFLOAT3 up{};//forwardの値をfloat3に
	DirectX::XMFLOAT3 d_vec{};//dの値をfloat3に
	DirectX::XMStoreFloat3(&forward, Forward);
	DirectX::XMStoreFloat3(&up, Up);
	DirectX::XMStoreFloat3(&d_vec, TargeVecNorm);

	//回転
	{
		//回転軸
		DirectX::XMVECTOR axis = Up;
		//回転角度がこの値を超えたときのみ計算
		const float extrapolated_angle = 5.0f;
		if (fabs(lock_on_angle) > DirectX::XMConvertToRadians(extrapolated_angle))
		{
			float cross{ forward.x * d_vec.z - forward.z * d_vec.x };
			//クオータニオンは回転の仕方(どの向きに)
			if (cross < 0.0f)
			{
				//回転軸（axis）と回転角（axis）から回転クオータニオン（q）を求める
				DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(axis, lock_on_angle);
				//矢印を徐々に目標座標に向ける
				DirectX::XMVECTOR  q2 = DirectX::XMQuaternionMultiply(orientationVec, q);
				orientationVec = DirectX::XMQuaternionSlerp(orientationVec, q2, lock_on_rate * elapsed_time);
			}
			else
			{
				//回転軸（axis）と回転角（axis）から回転クオータニオン（q）を求める
				DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(axis, -lock_on_angle);
				//矢印を徐々に目標座標に向ける
				DirectX::XMVECTOR  q2 = DirectX::XMQuaternionMultiply(orientationVec, q);
				orientationVec = DirectX::XMQuaternionSlerp(orientationVec, q2, lock_on_rate * elapsed_time);
			}
		}


	}
	// orientationVecからorientationを更新
	XMStoreFloat4(&orientation, orientationVec);
}
//==============================================================
// 
//パッドによるカメラ操作
// 
//==============================================================
void Camera::control_by_game_pad_stick(float elapsed_time)
{
	GamePad& game_pad = Device::instance().get_game_pad();
	float ax = game_pad.get_axis_RX();
	float ay = game_pad.get_axis_RY();
	//カメラ縦操作
	if (ay > 0.1f || ay < 0.1f)
	{
		angle.x = ay * DirectX::XMConvertToRadians(roll_speed) * elapsed_time;
	}
	//カメラ横操作
	if (ax > 0.1f || ax < 0.1f)
	{
		angle.y = ax * DirectX::XMConvertToRadians(roll_speed) * elapsed_time;
	}
	// XMVECTORクラスへ変換
	DirectX::XMVECTOR orientationVec = DirectX::XMLoadFloat4(&orientation);

	DirectX::XMVECTOR forward = Math::get_posture_forward_vec(orientation);
	DirectX::XMVECTOR up = { 0,1,0 };//カメラのY軸は常に（0,1,0）とする
	DirectX::XMVECTOR right = Math::get_posture_right_vec(orientation);

	//縦回転
	{
		//回転軸
		DirectX::XMVECTOR axis = DirectX::XMVector3Cross(forward, up);

		if (fabs(angle.x) > DirectX::XMConvertToRadians(0.1f))
		{
			//回転軸（axis）と回転角（axis）から回転クオータニオン（q）を求める
			DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(axis, angle.x);
			//矢印を徐々に目標座標に向ける
			DirectX::XMVECTOR  q2 = DirectX::XMQuaternionMultiply(orientationVec, q);
			orientationVec = DirectX::XMQuaternionSlerp(orientationVec, q2, sensitivity_rate);
		}

	}

	//横回転
	{
		//回転軸
		DirectX::XMVECTOR axis = up;

		if (fabs(angle.y) > DirectX::XMConvertToRadians(0.1f))
		{
			//回転軸（axis）と回転角（axis）から回転クオータニオン（q）を求める
			DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(axis, angle.y);
			//矢印を徐々に目標座標に向ける
			DirectX::XMVECTOR  q2 = DirectX::XMQuaternionMultiply(orientationVec, q);
			orientationVec = DirectX::XMQuaternionSlerp(orientationVec, q2, sensitivity_rate);
		}

	}
	//------------------------------------
	//カメラが上か下を向きすぎたときに補正する
	//------------------------------------

	//向きすぎと判断する値
	const float overdirection = 0.4f;
	if (Math::get_posture_up(orientation).y < overdirection)
	{
		if (Math::get_posture_forward(orientation).y < 0.0f)
		{
			float correction_rate = -5.0f;
			DirectX::XMVECTOR correct_angles_axis = DirectX::XMQuaternionRotationAxis(right, DirectX::XMConvertToRadians(correction_rate));
			orientationVec = DirectX::XMQuaternionMultiply(orientationVec, correct_angles_axis);
		}
		else
		{
			float correction_rate = 5.0f;
			DirectX::XMVECTOR correct_angles_axis = DirectX::XMQuaternionRotationAxis(right, DirectX::XMConvertToRadians(correction_rate));
			orientationVec = DirectX::XMQuaternionMultiply(orientationVec, correct_angles_axis);
		}
	}

	// orientationVecからorientationを更新
	XMStoreFloat4(&orientation, orientationVec);
}

//==============================================================
// 
//カメラシェイク
// 
//==============================================================
void Camera::camera_shake_update(float elapsed_time)
{
	DirectX::XMVECTOR orientationVec = DirectX::XMLoadFloat4(&orientation);
	DirectX::XMVECTOR standard_orientationVec = DirectX::XMLoadFloat4(&standard_orientation);


	//カメラシェイク効果中でありプレイヤーやロックオンなどのによるカメラへの力が加わっていない場合のみ揺らす
	if (is_camera_shake && fabs(angle.x) <= 0 && fabs(angle.y) <= 0 && fabs(lock_on_angle) <= 0)
	{
		//時間更新
		camera_shake_param.time -= elapsed_time;

		if (camera_shake_param.time < 0.0f)
		{
			is_camera_shake = false;
			camera_shake_param.time = 0.0f;
			XMStoreFloat4(&orientation, standard_orientationVec);
			return;
		}

		//揺らす処理
		{
			DirectX::XMVECTOR forward = Math::get_posture_forward_vec(standard_orientation);
			DirectX::XMVECTOR up = { 0,1,0 };
			DirectX::XMVECTOR right = Math::get_posture_right_vec(standard_orientation);

			//縦回転
			if (camera_shake_param.max_y_shake > 0)
			{
				//任意の揺れ幅の最大値最小値の間でのランダム生成
				float shake = Noise::instance().random_range(-camera_shake_param.max_y_shake, camera_shake_param.max_y_shake);
				shake = DirectX::XMConvertToRadians(shake);
				{
					//回転軸
					DirectX::XMVECTOR axis = right;

					if (fabs(shake) > DirectX::XMConvertToRadians(0.01f))
					{
						//回転軸（axis）と回転角（axis）から回転クオータニオン（q）を求める
						DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(axis, shake);

						standard_orientationVec = DirectX::XMQuaternionMultiply(standard_orientationVec, q);
					}

				}
			}

			//横揺れ
			if (camera_shake_param.max_x_shake > 0)
			{
				//任意の揺れ幅の最大値最小値の間でのランダム生成
				float shake = Noise::instance().random_range(-camera_shake_param.max_x_shake, camera_shake_param.max_x_shake);
				shake = DirectX::XMConvertToRadians(shake);
				{
					//回転軸
					DirectX::XMVECTOR axis = up;

					if (fabs(shake) > DirectX::XMConvertToRadians(0.01f))
					{
						//回転軸（axis）と回転角（axis）から回転クオータニオン（q）を求める
						DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(axis, shake);

						standard_orientationVec = DirectX::XMQuaternionMultiply(standard_orientationVec, q);
					}
				}
			}
		}
		orientationVec = DirectX::XMQuaternionSlerp(orientationVec, standard_orientationVec, camera_shake_param.shake_smoothness);
		// orientationVecからorientationを更新
		XMStoreFloat4(&orientation, orientationVec);

		//XMStoreFloat4(&standard_orientation, orientationVec);

	}
	else
	{
		//揺れがない間は基準値
		XMStoreFloat4(&standard_orientation, orientationVec);
	}

}
//==============================================================
// 
//ビュープロジェクションの計算
// 
//==============================================================
void Camera::calc_view_projection(float elapsed_time)
{
	// ビュー行列/プロジェクション行列を作成
	DirectX::XMMATRIX V = XMLoadFloat4x4(&view);
	DirectX::XMMATRIX P = XMLoadFloat4x4(&projection);
	// 定数バッファにフェッチする
	XMStoreFloat4x4(&scene_constant_buffer->data.view, V);
	XMStoreFloat4x4(&scene_constant_buffer->data.projection, P);
	XMStoreFloat4x4(&scene_constant_buffer->data.view_projection, V * P);
	scene_constant_buffer->data.light_color = light_color;
	scene_constant_buffer->data.light_direction = light_direction;
	scene_constant_buffer->data.camera_position = DirectX::XMFLOAT4(eye.x, eye.y, eye.z, 0);
	scene_constant_buffer->data.avatar_position = DirectX::XMFLOAT4(trakking_target.x, trakking_target.y, trakking_target.z, 0);
	scene_constant_buffer->data.resolution = { SCREEN_WIDTH,SCREEN_HEIGHT };
	scene_constant_buffer->data.time += elapsed_time;
	scene_constant_buffer->data.delta_time = elapsed_time;
	//すべてのシェーダーで使う可能性があるものなので全部に転送
	scene_constant_buffer->bind(Graphics::instance().get_dc().Get(), 3, CB_FLAG::ALL);

}
//==============================================================
// 
//デバッグGUI
// 
//==============================================================
void Camera::debug_gui()
{

#ifdef USE_IMGUI
	imgui_menu_bar("Camera", "main_camera", display_camera_imgui);
	if (display_camera_imgui)
	{
		ImGui::Begin("main_camera");
		//カメラシェイク
		if (ImGui::CollapsingHeader("CameraShake"))
		{
			static CameraShakeParam debug_param;

			ImGui::DragFloat("shake_x", &debug_param.max_x_shake, 0.1f);
			ImGui::DragFloat("shake_y", &debug_param.max_y_shake, 0.1f);
			ImGui::DragFloat("time", &debug_param.time, 0.1f);
			ImGui::DragFloat("smmoth", &debug_param.shake_smoothness, 0.1f, 0.1f, 1.0f);
			ImGui::DragFloat4("orientation", &orientation.x);
			ImGui::DragFloat4("standard_orientation", &standard_orientation.x);
			if (ImGui::Button("camera_shake"))
			{
				set_camera_shake(debug_param);
			}
		}
		//カメラシェイク
		if (ImGui::CollapsingHeader("HitStop"))
		{
			static HitStopParam debug_param;

			ImGui::DragFloat("time", &debug_param.time, 0.1f);
			ImGui::DragFloat("stopping_strength", &debug_param.stopping_strength, 0.1f);
			if (ImGui::Button("hit_stop"))
			{
				set_hit_stop(debug_param);
			}
		}
		if (ImGui::CollapsingHeader("LockOn"))
		{
			ImGui::Checkbox("lock_on", &lock_on);
			ImGui::DragFloat3("lockon_target", &lock_on_target.x);
		}


		if (ImGui::CollapsingHeader("Update"))
		{
			if (ImGui::Button("tracking")) p_update = &Camera::update_with_tracking;
		}
		if (ImGui::CollapsingHeader("Param"))
		{

			ImGui::DragFloat("range", &range, 0.2f);
			DirectX::XMFLOAT3 a = { DirectX::XMConvertToDegrees(angle.x),DirectX::XMConvertToDegrees(angle.y),DirectX::XMConvertToDegrees(angle.z) };
			DirectX::XMFLOAT3 up = Math::get_posture_up(orientation);
			DirectX::XMFLOAT3 forward = Math::get_posture_forward(orientation);
			DirectX::XMFLOAT3 right = Math::get_posture_forward(orientation);
			ImGui::DragFloat2("angle", &a.x, 0.1f);
			ImGui::DragFloat3("up", &up.x, 0.1f);
			ImGui::DragFloat3("forward", &forward.x, 0.1f);
			ImGui::DragFloat3("right", &right.x, 0.1f);
			ImGui::DragFloat("cape_vision", &cape_vision, 0.1f);
			ImGui::DragFloat("attend_rate", &attend_rate, 0.1f);
			ImGui::DragFloat("roll_speed", &roll_speed, 0.1f);
			ImGui::DragFloat("lock_on_rate", &lock_on_rate, 0.1f);
			angle = { DirectX::XMConvertToRadians(a.x),DirectX::XMConvertToRadians(a.y),DirectX::XMConvertToRadians(a.z) };
			ImGui::DragFloat3("target", &trakking_target.x, 0.1f);
			ImGui::DragFloat4("LightDirection", &light_direction.x, 0.01f, -1, 1);
			ImGui::DragFloat("time", &scene_constant_buffer->data.time, 0.01f, -1, 1);
			ImGui::DragFloat("delta_time", &scene_constant_buffer->data.delta_time, 0.01f, -1, 1);
		}
		/*if (ImGui::CollapsingHeader("color_picker", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::ColorPicker3("light_color", &light_color.x);
		}*/

		ImGui::End();
	}

#endif
}
//==============================================================
// 
//ヒットストップ
// 
//==============================================================
float Camera::hit_stop_update(float elapsed_time)
{
	float result_elapsed_tie = elapsed_time;
	if (is_hit_stop)
	{
		//ヒットストップ時の経過時間処理(0だとバグるので処理はスロー)
		result_elapsed_tie /= hit_stop_param.stopping_strength;

		//タイマー処理
		hit_stop_param.time -= elapsed_time;
		if (hit_stop_param.time < 0.0f)
		{
			is_hit_stop = false;
		}
	}

	return result_elapsed_tie;
}


void Camera::set_camera_shake(CameraShakeParam param)
{
	//カメラシェイクON
	is_camera_shake = true;
	//パラメーター設定
	camera_shake_param = param;
}

void Camera::set_hit_stop(HitStopParam param)
{
	is_hit_stop = true;
	hit_stop_param = param;
}

void Camera::calc_free_target()
{
	Mouse& mouse = Device::instance().get_mouse();

	float ax{};
	float ay{};
	{
		if (mouse.get_button() & (mouse.BTN_A | mouse.BTN_LEFT)) { ax = -1; }    //左移動
		if (mouse.get_button() & (mouse.BTN_D | mouse.BTN_RIGHT)) { ax = 1; }	 //右移動
		if (mouse.get_button() & (mouse.BTN_W | mouse.BTN_UP)) { ay = 1; }	 //前移動
		if (mouse.get_button() & (mouse.BTN_S | mouse.BTN_DOWN)) { ay = -1; }    //後ろ移動
	}
}



