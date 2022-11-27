#include "meteore.h"
#include "stage_manager.h"
#include "material_manager.h"
//==============================================================
// 
// コンストラクタ
// 
//==============================================================
Meteore::Meteore(Graphics& graphics, int max_num)
{
	main_effect = make_unique<InstanceMeshEffect>(graphics, "./resources/Effects/Meshes/meteore3.fbx", max_num);
	main_effect->set_material(MaterialManager::instance().mat_meteore.get());
	main_effect->constants->data.particle_color = { 4.0f, 1.0f, 0.7f, 0.8f };
	
	MAX_NUM = max_num;
	
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
	main_effect->stop();
	main_effect->play({ 0,0,0 });
	main_effect->set_is_loop(true);
	//隕石ごとのパラメーター初期化
	params.reset(new MeteoreParam[MAX_NUM]);
	for (int i = 0; i < MAX_NUM; i++)
	{
		params[i].position = { 0,0,0 };
		params[i].velocity = { 0,0,0 };
		params[i].scale = { 0,0,0 };
		params[i].is_calc_velocity = false;
		params[i].is_hit = false;
		main_effect->set_scale(0, i);
		main_effect->set_position({ 0,0,0 }, i);
	}
}
//==============================================================
// 
//更新処理
// 
//==============================================================
void Meteore::update(Graphics& graphics, float elapsed_time)
{
	main_effect->update(graphics,elapsed_time);


	for (int i = 0; i < MAX_NUM; i++)
	{
		main_effect->set_position(params[i].position, i);
		main_effect->set_scale(params[i].scale, i);
		//速度計算をするかどうか
		if (params[i].is_calc_velocity)
		{
			update_velocity(elapsed_time,i);
		};

		//仮でヒットした場合はサイズを０にする
		if (params[i].is_hit)
		{
			params[i].scale = Math::lerp(params[i].scale, { 0,0,0 }, 1.0f * elapsed_time);
			main_effect->set_scale(params[i].scale, i);
		}
	}
	
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

//==============================================================
// 
//円周上に隕石を配置
// 
//==============================================================
void Meteore::create_on_circle(DirectX::XMFLOAT3 center, float radius, int index)
{
	//隕石を円周上に配置
	params[index].position.x = Math::circumferential_placement({ center.x,center.z }, radius, index, MAX_NUM).x;
	params[index].position.y = center.y;
	params[index].position.z = Math::circumferential_placement({ center.x,center.z }, radius, index, MAX_NUM).y;
};

//==============================================================
// 
//指定の位置に上昇
// 
//==============================================================
void Meteore::rising(float elapsed_time, DirectX::XMFLOAT3 target_position, float target_scale, float rise_speed, int index)
{
	params[index].position = Math::lerp(params[index].position, target_position, rise_speed * elapsed_time);
	params[index].scale = Math::lerp(params[index].scale, { target_scale,target_scale,target_scale }, rise_speed * elapsed_time);
	DirectX::XMFLOAT3 s = params[index].scale;
	int a;
}
//==============================================================
// 
//一つずつ発射
// 
//==============================================================
void Meteore::launch(DirectX::XMFLOAT3 init_vec, float speed, int index)
{
	//すでに射出状態なら無視
	if (params[index].is_calc_velocity) return;
	//速度計算開始
	params[index].is_calc_velocity = true;
	params[index].is_hit = false;
	//発射方向設定
	move(init_vec.x, init_vec.z, speed, index);
}
//==============================================================
// 
//すべて一気に発射
// 
//==============================================================
void Meteore::all_launch(DirectX::XMFLOAT3 init_vec, float speed)
{
	for (int i = 0; i < MAX_NUM; i++)
	{
		params[i].is_calc_velocity = true;
		params[i].is_hit = false;
		main_effect->set_scale(0, i);
		move(init_vec.x, init_vec.z, speed, i);
	}
}
//==============================================================
// 
//速度更新
// 
//==============================================================
void Meteore::update_velocity(float elapsed_time, int index)
{
	//経過フレーム
	float elapsed_frame = 60.0f * elapsed_time;


	//垂直速力更新処理
	update_vertical_velocity(elapsed_frame, index);

	//垂直移動更新処理
	update_vertical_move(elapsed_time, index);

	//水平速力更新処理
	update_hrizontal_velocity(elapsed_frame, index);

	//水平移動更新処理
	update_horizontal_move(elapsed_time, index);

}
//==============================================================
// 
//移動方向の設定
// 
//==============================================================
void Meteore::move(float vx, float vz, float speed, int index)
{
	//方向設定
	params[index].move_vec.x = vx;
	params[index].move_vec.z = vz;

	//最大速度設定
	max_move_speed = speed;
}
//==============================================================
// 
//垂直方向の
// 
//==============================================================
void Meteore::update_vertical_velocity(float elapsed_frame, int index)
{
	params[index].velocity.y += gravity * elapsed_frame;

}

void Meteore::update_vertical_move(float elapsed_time, int index)
{
	// キャラクターの下方向の移動量
	float my = params[index].velocity.y * elapsed_time;


	// キャラクターのY軸方向となる法線ベクトル
	// 落下中
	if (my < 0.0f)
	{
		//レイの開始位置は少し上
		DirectX::XMFLOAT3 start = { params[index].position.x, params[index].position.y + 0.5f, params[index].position.z };
		//レイの終点位置は移動後の位置
		DirectX::XMFLOAT3 end = { params[index].position.x, params[index].position.y + my * ray_power  , params[index].position.z };

		//レイキャストによる地面判定
		HitResult hit;
		if (StageManager::Instance().ray_cast(start, end, hit))
		{
			//地面に設置している
			params[index].position = hit.position;

		
			//TODO:当たった時の処理に移行
			on_hit(index);
			//main_effect->stop();
			//動きを止める
			params[index].velocity = { 0,0,0 };
			params[index].move_vec.x = 0.0f;
			params[index].move_vec.z = 0.0f;
			
		}
		else
		{
			//空中に浮いている
			params[index].position.y += my;
		}

	}
	// 上昇中
	else if (my > 0.0f)
	{
		params[index].position.y += my;
	}

}

void Meteore::update_hrizontal_velocity(float elapsed_frame, int index)
{
	//XZ平面の速力
	float length = sqrtf(params[index].velocity.x * params[index].velocity.x + params[index].velocity.z * params[index].velocity.z);

	//XZ平面の速力を加速する
	if (length <= max_move_speed)
	{
		//移動ベクトルがゼロベクトルでないなら加速する
		float moveVecLength = sqrtf(params[index].move_vec.x * params[index].move_vec.x + params[index].move_vec.z * params[index].move_vec.z);


		if (moveVecLength > 0.0f)
		{
			//加速力
			float acceleration = this->acceleration * elapsed_frame;
			//空中にいるときは加速力を減らす
			//移動ベクトルによる加速処理
			params[index].velocity.x += params[index].move_vec.x * acceleration;
			params[index].velocity.z += params[index].move_vec.z * acceleration;

			//最大速度制限
			float length = sqrtf(params[index].velocity.x * params[index].velocity.x + params[index].velocity.z * params[index].velocity.z);
			if (length > max_move_speed)
			{
				float vx = params[index].velocity.x / length;
				float vz = params[index].velocity.z / length;
				params[index].velocity.x = vx * max_move_speed;
				params[index].velocity.z = vz * max_move_speed;
			}
		}
	}
	else
	{
		params[index].move_vec.x = 0.0f;
		params[index].move_vec.z = 0.0f;
	}
}

void Meteore::update_horizontal_move(float elapsed_time, int index)
{
	// 水平速力計算
	float velocity_length_xz = sqrtf(params[index].velocity.x * params[index].velocity.x + params[index].velocity.z * params[index].velocity.z);
	// もし速力があれば
	if (velocity_length_xz > 0.0f)
	{

		// 水平移動値
		float mx = params[index].velocity.x * elapsed_time;
		float mz = params[index].velocity.z * elapsed_time;

		// レイの開始位置と終点位置
		DirectX::XMFLOAT3 start = { params[index].position.x - mx / 50.0f, params[index].position.y * 2, params[index].position.z - mz / 50.0f };
		DirectX::XMFLOAT3 end = { params[index].position.x + mx * ray_power, start.y, params[index].position.z + mz * ray_power };
		HitResult hit;
		if (StageManager::Instance().ray_cast(start, end, hit))//何か壁があれば
		{

			//TODO:当たった時の処理に移行
			on_hit(index);

			params[index].position.x = hit.position.x;
			params[index].position.z = hit.position.z;
			params[index].velocity.x = 0;
			params[index].velocity.z = 0;			
		}
		else
		{
			//移動
			params[index].position.x += mx;
			params[index].position.z += mz;
		}

	}
}

void Meteore::on_hit(int index)
{
	params[index].is_calc_velocity = false;
	params[index].is_hit = true;
	//params[index].scale = { 0,0,0 };
}
