#include "skill_slash_wave.h"
#include "light_manager.h"
#include "Operators.h"
SlashWave::SlashWave(DirectX::XMFLOAT3* launch_pos, DirectX::XMFLOAT3* dir, PublicParam initparam)
{
	initialize();

	//各初期化パラメーター設定
	skill_duration = 1.5f;
	launch_position.reset(launch_pos);
	target_direction.reset(dir);
	//エフェクト
	{
		slash_mesh = std::make_unique<MeshEffect>("./resources/Effects/Meshes/eff_slash.fbx");
		slash_mesh->set_material(MaterialManager::instance().mat_fire_distortion.get());
		slash_mesh->set_init_scale(0.5f);
		slash_mesh->set_life_duration(0.2f);
		slash_mesh->set_init_color({ 1.0f,0.8f,5.5f,1.0f });
	}

	param = initparam;

	//ライト生成
	slash_light = make_shared<PointLight>(*launch_pos, 30.0f, DirectX::XMFLOAT3(1.0f, 0.8f, 5.5f));
	LightManager::instance().register_light("SlashWave", slash_light);

}

SlashWave::~SlashWave()
{
	launch_position.release();
	target_direction.release();
	//ライト消去
	LightManager::instance().delete_light(slash_light->name);

}

void SlashWave::initialize()
{
	life_time = 0.5f;
	collision_type = CollisionType::SPHERE;

}

void SlashWave::update(float elapsed_time)
{
	slash_mesh->update(elapsed_time);

	//アニメーションフレームが特定の値に達したら弾射出
	if (life_time > 1.0f)
	{
		if (!slash_mesh->get_active())
		{
			position = *launch_position;
			slash_mesh->play(position);
		}

		velocity = param.speed * Math::Normalize(*target_direction);

		position += velocity * elapsed_time;;
		attack_colider.start = position;
		attack_colider.end = position;
		attack_colider.radius = param.collider_radius;
		slash_mesh->set_position(position);
		slash_mesh->rotate_base_axis(MeshEffect::AXIS::RIGHT, Math::Normalize(-*target_direction));

		//ライト位置更新
		slash_light->set_position(position);
	}

	//消滅処理
	life_time += elapsed_time;
	if (life_time > skill_duration) skill_end_flag = true;

}

void SlashWave::render(Camera* camera)
{
	slash_mesh->render(camera);
}

void SlashWave::debug_gui(string str_id)
{
}
