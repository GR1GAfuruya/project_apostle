#include "skill_spears_sea.h"
#include "noise.h"
#include "user.h"
#include "light_manager.h"
//==============================================================
// 
//コンストラクタ
// 
//==============================================================
SpearsSea::SpearsSea(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos, PublicParam initparam)
{
	//パラメーター初期化
	initialize(graphics);


	instance_mesh = std::make_unique<InstanceMeshEffect>(graphics, "./resources/Effects/Meshes/eff_spear.fbx",MAX_NUM);
	instance_mesh->set_material(MaterialManager::instance().mat_fire_distortion.get());
	instance_mesh->constants->data.particle_color = { 1.0f,0.8f,5.5f,1.0f };
	position = launch_pos;
	target_position = target_pos;
	power = initparam.power;
	invinsible_time = initparam.invinsible_time;
	cool_time = initparam.cool_time;
	param = initparam;
	collision_type = CollisionType::SPHERE;
	//寿命を設定
	life_span = initparam.life_span;

	instance_mesh->constants->data.threshold = 0.0f;

	for (int i = 0; i < MAX_NUM; i++)
	{
		//---槍を徐々に伸ばす---//
		//伸ばす
		instance_mesh->set_scale(0, i);
	}


}
//==============================================================
// 
//デストラクタ
// 
//==============================================================
SpearsSea::~SpearsSea()
{
	LightManager::instance().delete_light(spear_light->name);
}
//==============================================================
// 
//初期化
// 
//==============================================================
void SpearsSea::initialize(Graphics& graphics)
{
	life_span = 0;
	finish = false;
	emit_num = 0;
	target_position = {};
	init_flag = false;
	init_emit_num = 10;
};
//==============================================================
// 
//更新処理
// 
//==============================================================
void SpearsSea::update(Graphics& graphics, float elapsed_time)
{
	life_time += elapsed_time;
	//槍エフェクトの再生
	if (life_time > 0.5f)
	{

		//各槍の出現位置
		DirectX::XMFLOAT3 appearance_pos{};
		instance_mesh->play(appearance_pos);
		//追尾時間が一定に達することでフィニッシュ状態へ
		if ( follow_timer < param.follow_time)
		{
			//タイマーを進める
			follow_timer += elapsed_time;
			position = target_position;
			//少し下に埋める
			position.y = target_position.y - 2.0f;
		}
		else
		{

			//攻撃の当たり判定パラメーター設定
			attack_colider.start = { position.x,position.y,position.z };
			attack_colider.radius = param.radius;
			//フィニッシュ時は槍を一気にすべて出す
			if (!finish)
			{
				spear_emit(0,MAX_NUM, SPEAR_SIZE);
				finish = true;
				//ライト設置
				DirectX::XMFLOAT3 point_light_pos = { position.x,position.y + 10.0f,position.z };//槍の位置より少し上に配置
				spear_light = make_shared<PointLight>(graphics, point_light_pos, 30.0f, DirectX::XMFLOAT3(1.0f, 0.8f, 5.5f));
				LightManager::instance().register_light("SpearsSea", spear_light);

			}

			//槍の更新処理
			const float deley_rate = 0.1f;
			for (int i = 0; i < MAX_NUM; i++)
			{
				//---槍を徐々に伸ばす---//
				//割合
				float rate = param.spear_length_rate * elapsed_time;
				//伸ばす
				param.spear_length = lerp(instance_mesh->get_scale(i).z, SPEAR_SIZE, rate);
				instance_mesh->set_scale({ SPEAR_SIZE,SPEAR_SIZE,param.spear_length }, i);
			}


		}

	}


	const float threshold_rate = 2.0f;
	const float start_threshold_time = 0.5f;
	//ディゾルブ処理
	if(life_time > life_span - start_threshold_time)
	{
		if (instance_mesh->constants->data.threshold <= 1.0f)
		{
			instance_mesh->constants->data.threshold += threshold_rate * elapsed_time;
		}
	}


	if (life_time > life_span) skill_end_flag = true;
}
//==============================================================
// 
//描画
// 
//==============================================================
void SpearsSea::render(Graphics& graphics)
{
	instance_mesh->render(graphics);
}


//==============================================================
// 
//槍出現
// 
//==============================================================
void SpearsSea::spear_emit(int index_offset, int emit_max_num, float size)
{
	DirectX::XMFLOAT3 appearance_pos{};
	for (int i = 0; i < emit_max_num; i++)
	{
		int random = std::abs(static_cast<int>(Noise::instance().get_rnd()));
		float circle_radius = random % static_cast<int>(param.radius);
		appearance_pos.x = Math::circumferential_placement({ position.x,position.z }, circle_radius, emit_num, MAX_NUM).x;
		appearance_pos.y = position.y - 3.0f;
		appearance_pos.z = Math::circumferential_placement({ position.x,position.z }, circle_radius, emit_num, MAX_NUM).y;
		//ばらばらに生やす
		DirectX::XMFLOAT3 spear_dir = Math::Normalize(DirectX::XMFLOAT3(cosf(emit_num), 1.01f, sinf(emit_num)));
		instance_mesh->set_position(appearance_pos, emit_num);
		instance_mesh->set_scale({ size,size,0 }, emit_num);
		instance_mesh->set_orientation(instance_mesh->rotate_base_axis(InstanceMeshEffect::AXIS::FORWARD, spear_dir, emit_num), emit_num);
		instance_mesh->set_is_loop(true);
		emit_num++;

		if (emit_num >= MAX_NUM) emit_num = index_offset;
	}
}
//==============================================================
// 
//デバッグGUI
// 
//==============================================================
void SpearsSea::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "SpearsSea:" + str_id + "Param";
	string skill_name = "SpearsSea";
	ImGui::Begin(skill_name.c_str());

	ImGui::PushID(str_id.c_str());
	/*これより下にパラメーター記述*/
	//ImGui::BulletText(name.c_str());
	ImGui::DragFloat("life_time", &life_time);
	ImGui::DragFloat("threshold", &instance_mesh->constants->data.threshold,0.01f);
	ImGui::DragInt("emit_num", &emit_num,0.1f);
	ImGui::DragFloat3("pos", &position.x,0.1f);
	ImGui::DragFloat3("target_position", &target_position.x,0.1f);
	if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		instance_mesh->debug_gui(skill_name);
	}

	/*これより上にパラメーター記述*/
	ImGui::PopID();
	ImGui::End();


#endif
}
