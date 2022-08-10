#include "framework.h"
#include "camera.h"
#include "imgui_include.h"

#include "user.h"
Camera::Camera(Graphics& graphics)
	: range(50.0f)
	, eye(5, 5, 5)
	, angle(DirectX::XMConvertToRadians(14.0f), DirectX::XMConvertToRadians(90.0f), 0)
	, target(30.0f, 10, 0)
	, light_direction(0.6f, -1, 0.1f, 0.7f)
	, roll_speed(DirectX::XMConvertToRadians(90))
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
		target = trakking_target;
	}
}

void Camera::update(float elapsed_time)
{
	using namespace DirectX;
	// 任意のアップデートを実行
	(this->*p_update)(elapsed_time);

	//カメラのパラメータ設定(全アップデート共通)
	{
		DirectX::XMFLOAT3 up(0, 1, 0);
		XMVECTOR eye_vec = XMLoadFloat3(&eye);
		XMVECTOR focus_vec = XMLoadFloat3(&target);
		XMVECTOR up_vec = XMLoadFloat3(&up);
		XMMATRIX view_mat = XMMatrixLookAtLH(eye_vec, focus_vec, up_vec);
		XMStoreFloat4x4(&view, view_mat);

		// ビューを逆行列化
		DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, view_mat);
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, World);

		// カメラの方向を取り出す
		this->right.x = world._11;
		this->right.y = world._12;
		this->right.z = world._13;

		up.x = world._21;
		up.y = world._22;
		up.z = world._23;

		this->forward.x = world._31;
		this->forward.y = world._32;
		this->forward.z = world._33;

		// プロジェクション行列を作成
		float width = static_cast<float>(SCREEN_WIDTH);
		float height = static_cast<float>(SCREEN_HEIGHT);
		float aspect_ratio{ width / height };
		static DirectX::XMFLOAT2 near_far = { 0.1f, 5000.0f };
		Mouse& mouse = Device::instance().get_mouse();
		if (mouse.get_button() & mouse.BTN_I) angle.x += DirectX::XMConvertToRadians(100) * elapsed_time;
		if (mouse.get_button() & mouse.BTN_K) angle.x -= DirectX::XMConvertToRadians(100) * elapsed_time;
		if (mouse.get_button() & mouse.BTN_L) angle.y += DirectX::XMConvertToRadians(100) * elapsed_time;
		if (mouse.get_button() & mouse.BTN_J) angle.y -= DirectX::XMConvertToRadians(100) * elapsed_time;
			
#ifdef USE_IMGUI
		if (display_camera_imgui)
		{
			ImGui::Begin("Camera");
			ImGui::DragFloat2("near_far", &near_far.x, 0.1f, 0.1f, 2000.0f);
			ImGui::End();
		}
#endif // USE_IMGUI
		XMMATRIX projection_mat = XMMatrixPerspectiveFovLH(XMConvertToRadians(cape_vision), aspect_ratio, near_far.x, near_far.y); // P
		XMStoreFloat4x4(&projection, projection_mat);
	}
}



void Camera::update_with_tracking(float elapsed_time)
{
	//X軸のカメラ回転を制限
	//clamp(angle.x, minAngleX, maxAngleX);

	//Y軸の回転値を-3.14~3.14に収まるようにする
	if (angle.y < -DirectX::XM_PI) { angle.y += DirectX::XM_2PI; }
	if (angle.y > DirectX::XM_PI) { angle.y -= DirectX::XM_2PI; }
	//カメラ回転値を回転行列に変換
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//回転行列の前方向ベクトルを取り出す
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);
	//注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	DirectX::XMFLOAT3 pos;
	pos.x = trakking_target.x - front.x * range;
	pos.y = trakking_target.y - front.y * range;
	pos.z = trakking_target.z - front.z * range;
	eye = Math::lerp(eye, pos, attend_rate * elapsed_time);
	target = trakking_target;
}

void Camera::update_with_euler_angles(float elapsed_time)
{
	using namespace DirectX;
	// X軸のカメラ回転を制限
	angle.x = std::clamp(angle.x, min_angle_x, max_angle_x);
	// Y軸の回転値を-3.14~3.14に収まるようにする
	if (angle.y < -DirectX::XM_PI) { angle.y += DirectX::XM_2PI; }
	if (angle.y > DirectX::XM_PI) { angle.y -= DirectX::XM_2PI; }
	// rangeの範囲制御
	range = std::clamp(range, 2.0f, 95.0f);
	// カメラ回転値を回転行列に変換
	DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	// 回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR forward_vec = transform.r[2];
	XMFLOAT3 forward;
	XMStoreFloat3(&forward, forward_vec);
	// 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	eye.x = target.x - forward.x * range;
	eye.y = target.y - forward.y * range;
	eye.z = target.z - forward.z * range;
#ifdef USE_IMGUI

#endif // USE_IMGUI

}

void Camera::update_with_quaternion(float elapsed_time)
{
	using namespace DirectX;
	// rangeの範囲制御
	range = std::clamp(range, 0.5f, 95.0f);
	// カメラ回転値を回転行列に変換
	// XMVECTORクラスへ変換
	XMVECTOR orientation_vec = XMLoadFloat4(&orientation);
	//回転行列から各方向を出す
	XMVECTOR forward_vec, right_vec, up_vec;
	XMMATRIX m = XMMatrixRotationQuaternion(orientation_vec);
	XMFLOAT4X4 m4x4 = {};
	XMStoreFloat4x4(&m4x4, m);
	right_vec = { m4x4._11, m4x4._12, m4x4._13 };
	XMFLOAT3 up{ 0,1,0 };
	up_vec = XMLoadFloat3(&up);
	forward_vec = { m4x4._31, m4x4._32, m4x4._33 };
	XMFLOAT3 forward;
	XMStoreFloat3(&forward, XMVector3Normalize(forward_vec));
	// 回転の制御
	XMVECTOR dot_vec = XMVector3Dot(XMVector3Normalize(forward_vec), up_vec);
	float dot;
	XMStoreFloat(&dot, dot_vec);
	// マウス操作でカメラ操作
	// 上下の回転角の制御
	float angle = XMConvertToDegrees(acosf(dot));
	if (angle >= 170 && vertical_rotation_degree > 0)
	{
		vertical_rotation_degree = static_cast<float>((std::min)(0, (int)vertical_rotation_degree));
	}
	if (angle <= 10 && vertical_rotation_degree < 0)
	{
		vertical_rotation_degree = static_cast<float>((std::max)(0, (int)vertical_rotation_degree));
	}
	// 上下
	{
		DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(right_vec, DirectX::XMConvertToRadians(vertical_rotation_degree));
		orientation_vec = DirectX::XMQuaternionMultiply(orientation_vec, q);
		vertical_rotation_degree = 0;
	}
	// 左右
	{
		DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(up_vec, DirectX::XMConvertToRadians(horizon_rotation_degree));
		orientation_vec = DirectX::XMQuaternionMultiply(orientation_vec, q);
		horizon_rotation_degree = 0;
	}
	// orientation更新
	XMStoreFloat4(&orientation, orientation_vec);
	// 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	//XMFLOAT3 eye;
	eye.x = target.x - forward.x * range;
	eye.y = target.y - forward.y * range;
	eye.z = target.z - forward.z * range;

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
	scene_constant_buffer->data.avatar_position = DirectX::XMFLOAT4(target.x, target.y, target.z, 0);
	scene_constant_buffer->data.resolution = { SCREEN_WIDTH,SCREEN_HEIGHT };
	scene_constant_buffer->data.time += elapsed_time;
	scene_constant_buffer->data.delta_time = elapsed_time;
	//すべてのシェーダーで使う可能性があるものなので全部に転送
	scene_constant_buffer->bind(graphics.get_dc().Get(), 3,CB_FLAG::ALL);
	
}

void Camera::debug_gui()
{

#ifdef USE_IMGUI
	if (display_camera_imgui)
	{
		ImGui::Begin("Camera");
		if (ImGui::Button("tracking")) p_update = &Camera::update_with_tracking;
		if (ImGui::Button("euler_angles")) p_update = &Camera::update_with_euler_angles;
		if (ImGui::Button("quaternion")) p_update = &Camera::update_with_quaternion;

		ImGui::DragFloat("range", &range, 0.2f);
		DirectX::XMFLOAT3 a = { DirectX::XMConvertToDegrees(angle.x),DirectX::XMConvertToDegrees(angle.y),DirectX::XMConvertToDegrees(angle.z) };
		ImGui::DragFloat2("angle", &a.x, 0.1f);
		angle = { DirectX::XMConvertToRadians(a.x),DirectX::XMConvertToRadians(a.y),DirectX::XMConvertToRadians(a.z) };
		ImGui::DragFloat3("target", &target.x, 0.1f);
		ImGui::DragFloat4("LightDirection", &light_direction.x, 0.01f, -1, 1);
		ImGui::DragFloat("time", &scene_constant_buffer->data.time, 0.01f, -1, 1);
		ImGui::DragFloat("time", &scene_constant_buffer->data.delta_time, 0.01f, -1, 1);
		if (ImGui::CollapsingHeader("color_picker", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::ColorPicker3("light_color", &light_color.x);
		}

		ImGui::End();
	}

#endif
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


