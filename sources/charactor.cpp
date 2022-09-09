#include"charactor.h"

#include "stage_manager.h"
#include "user.h"

//行列更新処理
void Charactor::update_transform()
{
	//スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転行列を作成
	//DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	//DirectX::XMMATRIX R = Y * X * Z;
	DirectX::XMMATRIX R = Y * X * Z;
	//位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//3つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	//計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}

//クオータニオン用のUpdateTransform
void Charactor::update_transform(const DirectX::XMFLOAT4& orien, const DirectX::XMFLOAT3& pos)
{
	//スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&orien));
	//位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//3つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	//計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);

}


//SceneGrath 親子行列
#if 0
void Character::update_transform(const DirectX::XMFLOAT4& orien, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT4X4& tran)
{
	//スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&orien));
	//位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x - pos.x, position.y - pos.y, position.z - pos.z);
	//3つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMMATRIX tra = DirectX::XMLoadFloat4x4(&tran);
	W = tra * W;
	//計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}
#endif // 0


void Charactor::add_impulse(const DirectX::XMFLOAT3& impulse)
{
	//速力に力を加える
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;

}

bool Charactor::apply_damage(int damage, float invinsibleTime)
{
	//ダメージが0の場合は健康状態を変更する必要がない
	if (damage == 0)return false;

	//死亡している場合は健康状態を変更しない
	if (health <= 0)return false;


	if (invincible_timer > 0.0f)return false;

	//無敵時間設定
	invincible_timer = invinsibleTime;
	//ダメージ処理
	health -= damage;

	//死亡通知
	if (health <= 0)
	{
		//OnDead();
	}
	else//ダメージ通知
	{
		//on_damaged();
	}

	//健康状態が変更した場合はtrueを返す
	return true;
}





void Charactor::Move(float vx, float vz, float speed)
{
	//移動方向ベクトルを設定
	move_vec_x = vx;
	move_vec_z = vz;

	//最大速度設定
	max_move_speed = speed;
}

void Charactor::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;
	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f) return;

	vx /= length;
	vz /= length;


	const float forwardX = sinf(angle.y);
	const float forwardZ = cosf(angle.y);

	//ベクトルＡとＢの外積
	//A.x*B.z - A.z*B.x
	float cross = forwardX * vz - forwardZ * vx;

	//ベクトルＡとＢの内積
	//A.x*B.x - A.z*B.z
	float dot = forwardX * vx + forwardZ * vz;
	float rot = 1 - dot;

	if (rot > speed) rot = speed;


	if (cross < 0.0f)//右に旋回
	{
		angle.y += rot;
	}

	else //左に旋回
	{
		angle.y -= rot;
	}

	angle.x = fmod(angle.x, DirectX::XMConvertToRadians(360.0f));
	angle.y = fmod(angle.y, DirectX::XMConvertToRadians(360.0f));
	angle.z = fmod(angle.z, DirectX::XMConvertToRadians(360.0f));
}

void Charactor::Turn(float elapsed_time, DirectX::XMFLOAT3 move_vec, float speed, DirectX::XMFLOAT4& orien)
{
	// XMVECTORクラスへ変換
	DirectX::XMVECTOR orientationVec = DirectX::XMLoadFloat4(&orien);
	DirectX::XMVECTOR MoveVec = DirectX::XMLoadFloat3(&move_vec);
	if (DirectX::XMVector3Equal(MoveVec, DirectX::XMVectorZero())) return; //もしmove_vecがゼロベクトルならリターン
	DirectX::XMVECTOR forward, up;

	up = Math::get_posture_up_vec(orien);
	forward = Math::get_posture_forward_vec(orien);

	up = DirectX::XMVector3Normalize(up);
	forward = DirectX::XMVector3Normalize(forward);

	//正規化
	MoveVec = DirectX::XMVector3Normalize(MoveVec);

	DirectX::XMVECTOR axis;	//回転軸
	float angle;			//回転角
	axis = up;
	DirectX::XMVECTOR Ang = DirectX::XMVector3Dot(forward, MoveVec);
	DirectX::XMStoreFloat(&angle, Ang);
	angle = acosf(angle);

	DirectX::XMFLOAT3 forw{};//forwardの値をfloat3に
	DirectX::XMFLOAT3 m_vec{};//dの値をfloat3に

	DirectX::XMStoreFloat3(&forw, forward);
	DirectX::XMStoreFloat3(&m_vec, MoveVec);
	//回転角（angle）が微小な場合は、回転を行わない
	if (fabs(angle) > 1e-8f)
	{
		//回転軸（axis）と回転角（angle）から回転クオータニオン（q）を求める
		float cross{ forw.x * m_vec.z - forw.z * m_vec.x };

		//クオータニオンは回転の仕方(どの向きに)
		const float rate = 10.0f;
		if (cross < 0.0f)
		{
			//回転軸と回転角から回転クオータニオンを求める
			DirectX::XMVECTOR q;
			q = DirectX::XMQuaternionRotationAxis(axis, angle);//正の方向に動くクオータニオン

			DirectX::XMVECTOR End = DirectX::XMQuaternionMultiply(orientationVec, q);
			orientationVec = DirectX::XMQuaternionSlerp(orientationVec, End, rate * elapsed_time);
		}
		else
		{
			DirectX::XMVECTOR q;
			q = DirectX::XMQuaternionRotationAxis(axis, -angle);//負の方向に動くクオータニオン
			DirectX::XMVECTOR End = DirectX::XMQuaternionMultiply(orientationVec, q);
			orientationVec = DirectX::XMQuaternionSlerp(orientationVec, End, rate * elapsed_time);
		}

	}

	// orientationVecからorientationを更新
	DirectX::XMStoreFloat4(&orientation, orientationVec);
}

void Charactor::Jump(float speed)
{
	//上方向の力を設定
	velocity.y = speed;
}
void Charactor::update_velocity(float elapsedTime, DirectX::XMFLOAT3& position, Stage* stage)
{
	//経過フレーム
	float elapsedFrame = 60.0f * elapsedTime;


	//垂直速力更新処理
	update_vertical_velocity(elapsedFrame);

	//垂直移動更新処理
	update_vertical_move(elapsedTime, position, stage);

	//水平速力更新処理
	update_hrizontal_velocity(elapsedFrame);

	//水平移動更新処理
	update_horizontal_move(elapsedTime, position, stage);

}

void Charactor::update_vertical_velocity(float elapsedFrame)
{
	velocity.y += gravity * elapsedFrame;
}

using namespace DirectX;
void Charactor::update_vertical_move(float elapsed_time, DirectX::XMFLOAT3& position, Stage* stage)
{
	// キャラクターの下方向の移動量
	float my = velocity.y * elapsed_time;


	// キャラクターのY軸方向となる法線ベクトル
	slope_normal = { 0, 1, 0 };
	// 落下中
	if (my < 0.0f)
	{
		//レイの開始位置は足元より少し上
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		//レイの終点位置は移動後の位置
		DirectX::XMFLOAT3 end = { position.x, position.y + my  , position.z };

		//レイキャストによる地面判定
		HitResult hit;
		if (StageManager::Instance().ray_cast(start, end, hit))
		{
			//地面に設置している
			position = hit.position;

			//angle.y += hit.rotation.y;
			// 法線ベクトル取得
			//normal = hit.normal;
			//着地した
			if (!is_ground)
			{
				on_landing();
			}
			is_ground = true;
			velocity.y = 0.0f;
		}
		else
		{
			//空中に浮いている
			position.y += my;
			is_ground = false;
		}

	}
	// 上昇中
	else if (my > 0.0f)
	{
		position.y += my;
		is_ground = false;
	}

	// 地面の向きに沿うようにXZ軸回転
	{

		DirectX::XMVECTOR OrientationVec = DirectX::XMLoadFloat4(&orientation);

		//上ベクトル
		DirectX::XMVECTOR up = Math::get_posture_up_vec(orientation);
		//法線のベクトル
		DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&slope_normal);
		Normal = DirectX::XMVector3Normalize(Normal);
		//軸ベクトル算出
		DirectX::XMVECTOR axis;	//回転軸
		float angle;			//回転角
		axis = DirectX::XMVector3Cross(up, Normal);
		DirectX::XMVECTOR Ang = DirectX::XMVector3Dot(up, Normal);
		DirectX::XMStoreFloat(&angle, Ang);
		angle = acosf(angle);
		//軸がゼロベクトルならリターン
		if (DirectX::XMVector3Equal(axis, DirectX::XMVectorZero())) return;
		//回転角（angle）が微小な場合は、回転を行わない
		if (fabs(angle) > 1e-8f)
		{
			DirectX::XMVECTOR q;
			q = DirectX::XMQuaternionRotationAxis(axis, angle);
			DirectX::XMVECTOR End = DirectX::XMQuaternionMultiply(OrientationVec, q);
			float rate = 10.0f;
			OrientationVec = DirectX::XMQuaternionSlerp(OrientationVec, End, rate * elapsed_time);
		}
		// orientationVecからorientationを更新
		DirectX::XMStoreFloat4(&orientation, OrientationVec);

	}


}

void Charactor::update_hrizontal_velocity(float elapsed_frame)
{
	//XZ平面の速力を減速する
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length > 0.0f)
	{
		float friction = this->friction * elapsed_frame;

		//if (IsGround()) friction += airControl;
		//摩擦による横方向の減速処理
		if (length > friction)
		{
			float vx = velocity.x / length;
			float vz = velocity.z / length;
			velocity.x -= vx * friction;
			velocity.z -= vz * friction;
		}
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}

	}
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
			if (get_is_ground()) acceleration += air_control;
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


void Charactor::update_horizontal_move(float elapsed_time,DirectX::XMFLOAT3& position, Stage* stage)
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
		DirectX::XMFLOAT3 start = { position.x - mx / 50.0f, position.y + stepOffset, position.z - mz / 50.0f };
		DirectX::XMFLOAT3 end = { position.x + mx* 5.0f, start.y, position.z + mz * 5.0f};
		HitResult hit;
		if (StageManager::Instance().ray_cast(start, end, hit))//何か壁があれば
		{
			//壁までのベクトル
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

		DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

			//入射ベクトルを法線に射影
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal); //この時点では正負が逆なことに注意

			//補正位置の計算
			DirectX::XMVECTOR Correct = DirectX::XMVectorMultiplyAdd(Normal, Dot, End);//Endに射影ベクトルを足す



			DirectX::XMFLOAT3 correct;
			DirectX::XMStoreFloat3(&correct, Correct);

			HitResult hit2;
			if (!StageManager::Instance().ray_cast(start, correct, hit2))//何か壁があれば
			{
				position.x = correct.x;
				//velocity.x = 0;
				position.z = correct.z;
				//velocity.z = 0;
			}
			else
			{
				position.x = hit2.position.x;
				position.z = hit2.position.z;
			}
		}
		else
		{
			//移動
			position.x += mx;
			position.z += mz;
		}
		
	}
}



void Charactor::update_invicible_timer(float elapsedTime)
{
	if (invincible_timer > 0.0f)
	{
		invincible_timer -= elapsedTime;
	}
	else
	{
		invincible_timer = 0.0f;
	}
}
