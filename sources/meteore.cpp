#include "meteore.h"
#include "stage_manager.h"

//==============================================================
// 
// コンストラクタ
// 
//==============================================================
Meteore::Meteore(Graphics& graphics)
{
	main_effect = make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/meteore3.fbx");
	main_effect->register_shader_resource(graphics.get_device().Get(), L"./resources/Effects/Textures/Traill2_output.png");
	main_effect->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/Mask/dissolve_animation.png");
	main_effect->register_shader_resource(graphics.get_device().Get(), L"./resources/TexMaps/distortion.tga");
	main_effect->create_pixel_shader(graphics.get_device().Get(), "./shaders/fire_distortion.cso");
	main_effect->constants->data.particle_color = { 4.0f, 1.0f, 0.7f, 0.8f };


	//パラメーター初期化
	initialize();
}

//==============================================================
// 
// 初期化
// 
//==============================================================
void Meteore::initialize()
{
	acceleration = 15.0f;
	friction = 0.0f;
	max_move_speed = 40.0f;
}
//==============================================================
// 
//更新処理
// 
//==============================================================
void Meteore::update(Graphics& graphics, float elapsed_time)
{
	main_effect->set_position(position);
	main_effect->update(graphics,elapsed_time);

	if (!is_hit)
	{
		update_velocity(elapsed_time, position);
	};
}
//==============================================================
// 
//描画処理（ディファード）
// 
//==============================================================
void Meteore::render(Graphics& graphics)
{
	main_effect->render(graphics);
}

void Meteore::launch(DirectX::XMFLOAT3 init_pos, DirectX::XMFLOAT3 init_vec, float speed)
{
	main_effect->play(init_pos);
	main_effect->set_scale(3.0f);
	//初期位置設定
	position = init_pos;
	velocity = { 0,0,0 };
	//発射方向設定
	move(init_vec.x, init_vec.z, speed);
}

void Meteore::update_velocity(float elapsed_time, DirectX::XMFLOAT3& position)
{
	//経過フレーム
	float elapsed_frame = 60.0f * elapsed_time;


	//垂直速力更新処理
	update_vertical_velocity(elapsed_frame);

	//垂直移動更新処理
	update_vertical_move(elapsed_time, position);

	//水平速力更新処理
	update_hrizontal_velocity(elapsed_frame);

	//水平移動更新処理
	update_horizontal_move(elapsed_time, position);

}

void Meteore::move(float vx, float vz, float speed)
{
	//方向設定
	move_vec_x = vx;
	move_vec_z = vz;

	//最大速度設定
	max_move_speed = speed;
}

void Meteore::update_vertical_velocity(float elapsed_frame)
{
	velocity.y += gravity * elapsed_frame;

}

void Meteore::update_vertical_move(float elapsed_time, DirectX::XMFLOAT3& position)
{
	// キャラクターの下方向の移動量
	float my = velocity.y * elapsed_time;


	// キャラクターのY軸方向となる法線ベクトル
	// 落下中
	if (my < 0.0f)
	{
		//レイの開始位置は少し上
		DirectX::XMFLOAT3 start = { position.x, position.y + 0.5f, position.z };
		//レイの終点位置は移動後の位置
		DirectX::XMFLOAT3 end = { position.x, position.y + my * ray_power  , position.z };

		//レイキャストによる地面判定
		HitResult hit;
		if (StageManager::Instance().ray_cast(start, end, hit))
		{
			//地面に設置している
			position = hit.position;

		
			//TODO:当たった時の処理に移行
			if (!is_hit)
			{
				on_hit();
			}
			//main_effect->stop();
			//動きを止める
			velocity = { 0,0,0 };
			move_vec_x = 0.0f;
			move_vec_z = 0.0f;
			
		}
		else
		{
			//空中に浮いている
			position.y += my;
		}

	}
	// 上昇中
	else if (my > 0.0f)
	{
		position.y += my;
	}

}

void Meteore::update_hrizontal_velocity(float elapsed_frame)
{
	//XZ平面の速力
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

	//XZ平面の速力を加速する
	if (length <= max_move_speed)
	{
		//移動ベクトルがゼロベクトルでないなら加速する
		float moveVecLength = sqrtf(move_vec_x * move_vec_x + move_vec_z * move_vec_z);


		if (moveVecLength > 0.0f)
		{
			//加速力
			float acceleration = this->acceleration * elapsed_frame;
			//空中にいるときは加速力を減らす
			//移動ベクトルによる加速処理
			velocity.x += move_vec_x * acceleration;
			velocity.z += move_vec_z * acceleration;

			//最大速度制限
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > max_move_speed)
			{
				float vx = velocity.x / length;
				float vz = velocity.z / length;
				velocity.x = vx * max_move_speed;
				velocity.z = vz * max_move_speed;
			}
		}
	}
	else
	{
		move_vec_x = 0.0f;
		move_vec_z = 0.0f;
	}
}

void Meteore::update_horizontal_move(float elapsed_time, DirectX::XMFLOAT3& position)
{
	// 水平速力計算
	float velocity_length_xz = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	// もし速力があれば
	if (velocity_length_xz > 0.0f)
	{

		// 水平移動値
		float mx = velocity.x * elapsed_time;
		float mz = velocity.z * elapsed_time;

		// レイの開始位置と終点位置
		DirectX::XMFLOAT3 start = { position.x - mx / 50.0f, position.y * 2, position.z - mz / 50.0f };
		DirectX::XMFLOAT3 end = { position.x + mx * ray_power, start.y, position.z + mz * ray_power };
		HitResult hit;
		if (StageManager::Instance().ray_cast(start, end, hit))//何か壁があれば
		{

			//TODO:当たった時の処理に移行
			if (!is_hit)
			{
				on_hit();
			}

			position.x = hit.position.x;
			position.z = hit.position.z;
			velocity.x = 0;
			velocity.z = 0;			
		}
		else
		{
			//移動
			position.x += mx;
			position.z += mz;
		}

	}
}

void Meteore::on_hit()
{
	is_hit = true;
}
