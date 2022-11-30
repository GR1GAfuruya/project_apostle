#include "framework.h"
#include "camera.h"
#include "imgui_include.h"
#include "Operators.h"
#include "stage_manager.h"
Camera::Camera(Graphics& graphics)
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
	,post_effect(graphics.get_device().Get())
{
	HRESULT hr{ S_OK };
	//----定数バッファ----//
	// カメラ関連
	{
		scene_constant_buffer = std::make_unique<Constants<SCENE_CONSTANTS>>(graphics.get_device().Get());
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
}

void Camera::update(float elapsed_time)
{
	using namespace DirectX;
	// 任意のアップデートを実行
	(this->*p_update)(elapsed_time);

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

		// ビューを逆行列化
		DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, view_mat);
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, World);


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

	//カメラストップのタイマーの更新
	{
		if (camera_stop_timer > 0.0f)
		{
			camera_stop_timer -= elapsed_time;
		}
		else
		{
			//タイマーが切れたらcamera_stopをオフに
			camera_stop = false;
		}
	}
}



void Camera::update_with_tracking(float elapsed_time)
{
	if (lock_on)
	{
		//ロックオン時のカメラの挙動
		update_with_lock_on(elapsed_time, orientation);
		
	}
	else
	{
		//ロックオンしていないときのカメラの挙動
		control_by_game_pad_stick(elapsed_time, orientation);

	}
	//Y軸の回転値を-3.14~3.14に収まるようにする
	if (angle.y < -DirectX::XM_PI) { angle.y += DirectX::XM_2PI; }
	if (angle.y > DirectX::XM_PI) { angle.y -= DirectX::XM_2PI; }

	// カメラ回転値を回転行列に変換
	// XMVECTORクラスへ変換
	DirectX::XMFLOAT3 forward = Math::get_posture_forward(orientation);

	// レイキャスト(ターゲットと壁)
	DirectX::XMFLOAT3 ray_target = trakking_target + DirectX::XMFLOAT3{0,-0.5,0};//めり込まないよう少し下に下げる
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

void Camera::update_with_lock_on(float elapsed_time, DirectX::XMFLOAT4& ori)
{
	// XMVECTORクラスへ変換
	DirectX::XMVECTOR orientationVec = DirectX::XMLoadFloat4(&ori);

	// カメラの現在位置から、目標座標への方向を求める
	DirectX::XMFLOAT3 d = lock_on_target - eye;
	DirectX::XMVECTOR d_vec_norm = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&d));
	DirectX::XMVECTOR forward = Math::get_posture_forward_vec(ori);
	DirectX::XMVECTOR right = Math::get_posture_right_vec(ori);
	// 矢印の現在姿勢（orientation）における前方（forward）をホーム方向とし、
	// 方向への回転軸（axis）と回転角（angle）を求める
	// 回転角
	float angle;
	DirectX::XMVECTOR dot = DirectX::XMVector3Dot(DirectX::XMVector3Normalize(forward), d_vec_norm); // cos
	DirectX::XMStoreFloat(&angle, dot);
	angle = acosf(angle);
	//回転軸
	DirectX::XMVECTOR axis = DirectX::XMVector3Cross(forward, d_vec_norm);
	//回転角（angle）が微小な場合は、回転を行わない
	if (fabs(angle) > DirectX::XMConvertToRadians(0.1f))
	{
		//回転軸（axis）と回転角（axis）から回転クオータニオン（q）を求める
		DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(axis, angle);
		//矢印を徐々に目標座標に向ける
		DirectX::XMVECTOR  q2 = DirectX::XMQuaternionMultiply(orientationVec, q);
		orientationVec = DirectX::XMQuaternionSlerp(orientationVec, q2, lock_on_rate);
	}
	else
	{
		lock_on = false;
	}
	if (Math::get_posture_up(ori).y < 0.4f)
	{
		DirectX::XMVECTOR correct_angles_axis = DirectX::XMQuaternionRotationAxis(right, DirectX::XMConvertToRadians(-25.0f));
		orientationVec = DirectX::XMQuaternionMultiply(orientationVec, correct_angles_axis);
	}


	// orientationVecからorientationを更新
	XMStoreFloat4(&ori, orientationVec);

}

void Camera::control_by_game_pad_stick(float elapsed_time, DirectX::XMFLOAT4& ori)
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
	DirectX::XMVECTOR orientationVec = DirectX::XMLoadFloat4(&ori);

	DirectX::XMVECTOR forward = Math::get_posture_forward_vec(ori);
	DirectX::XMVECTOR up = Math::get_posture_up_vec(ori);
	DirectX::XMVECTOR right = Math::get_posture_right_vec(ori);

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
			orientationVec = DirectX::XMQuaternionSlerp(orientationVec, q2, lock_on_rate);
		}

	}

	//横回転
	{
		//回転軸
		DirectX::XMVECTOR axis = DirectX::XMVector3Cross(forward, right);

		if (fabs(angle.y) > DirectX::XMConvertToRadians(0.1f))
		{
			//回転軸（axis）と回転角（axis）から回転クオータニオン（q）を求める
			DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(axis, angle.y);
			//矢印を徐々に目標座標に向ける
			DirectX::XMVECTOR  q2 = DirectX::XMQuaternionMultiply(orientationVec, q);
			orientationVec = DirectX::XMQuaternionSlerp(orientationVec, q2, lock_on_rate);
		}

	}

	//カメラが上か下を向きすぎたときに補正する　※上を向きすぎたときの処理がうまくいってないので要修正）
	if (Math::get_posture_up(ori).y <0.4f)
	{
		float correction_rate = -15.0f;
		DirectX::XMVECTOR correct_angles_axis = DirectX::XMQuaternionRotationAxis(right, DirectX::XMConvertToRadians(correction_rate));
		orientationVec = DirectX::XMQuaternionMultiply(orientationVec, correct_angles_axis);
	}


	// orientationVecからorientationを更新
	XMStoreFloat4(&ori, orientationVec);

}


void Camera::calc_view_projection(Graphics& graphics, float elapsed_time)
{
	// ビュー行列/プロジェクション行列を作成
	DirectX::XMMATRIX V = XMLoadFloat4x4(&view);
	DirectX::XMMATRIX P = XMLoadFloat4x4(&projection);
	// 定数バッファにフェッチする
	XMStoreFloat4x4(&scene_constant_buffer->data.view, V );
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
	scene_constant_buffer->bind(graphics.get_dc().Get(), 3,CB_FLAG::ALL);
	
}

void Camera::debug_gui()
{

#ifdef USE_IMGUI
	imgui_menu_bar("Camera", "main_camera", display_camera_imgui);
	if (display_camera_imgui)
	{
		ImGui::Begin("main_camera");
		if (ImGui::Button("tracking")) p_update = &Camera::update_with_tracking;

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
		ImGui::DragFloat("roll_speed", &roll_speed);
		ImGui::DragFloat("lock_on_rate", &lock_on_rate
		
		);
		angle = { DirectX::XMConvertToRadians(a.x),DirectX::XMConvertToRadians(a.y),DirectX::XMConvertToRadians(a.z) };
		ImGui::DragFloat3("target", &trakking_target.x, 0.1f);
		ImGui::DragFloat4("LightDirection", &light_direction.x, 0.01f, -1, 1);
		ImGui::DragFloat("time", &scene_constant_buffer->data.time, 0.01f, -1, 1);
		ImGui::DragFloat("delta_time", &scene_constant_buffer->data.delta_time, 0.01f, -1, 1);
		/*if (ImGui::CollapsingHeader("color_picker", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::ColorPicker3("light_color", &light_color.x);
		}*/

		ImGui::End();
	}

#endif
}

void Camera::set_camera_stop(float stop_time)
{
	//カメラストップ状態でないなら
	if (!camera_stop)
	{
		//カメラを止め、タイマー設定
		camera_stop = true;
		camera_stop_timer = stop_time;
	}
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


