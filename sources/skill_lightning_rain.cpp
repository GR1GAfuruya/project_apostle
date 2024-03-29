#include "skill_lightning_rain.h"
#include "Operators.h"
LightningRain::LightningRain(DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos, PublicParam initparam)
{
	//初期化パラメーター
	{
		param = initparam;
		position = target_pos;
	}
	//エフェクト
	{
		lightning_mesh = std::make_unique<MeshEffect>("./resources/Effects/Meshes/lightning.fbx");
		lightning_mesh->set_init_life_duration(0.5f);
		lightning_mesh->set_material(MaterialManager::instance().mat_lightning.get());
		lightning_mesh->set_init_scale({ 2.5f,3.0f,2.5f });
		lightning_mesh->set_init_color({ 1.0f,4.0f,6.0f,1.0f });
	}
	//当たり判定初期化
	{
		skill_duration = 2.0f;
		attack_colider.radius = 0;
	}
}

LightningRain::~LightningRain()
{
}

void LightningRain::initialize()
{
}

void LightningRain::update(float elapsed_time)
{
	if (life_time > 1.5f)
	{
		//エフェクトがアクティブでないなら再生
		if (!lightning_mesh->get_active())
		{
			DirectX::XMFLOAT3 lightning_dir = { 0,1,0 };
			attack_colider.start = position;
			attack_colider.end = position + Math::vector_scale(lightning_dir, param.lightning_length);
			attack_colider.radius = param.collider_radius;

			auto lightning_pos = position;
			lightning_pos.y -= param.lightning_length;
			lightning_mesh->play(lightning_pos);
		}
	}
	else
	{
		if (!lightning_mesh->get_active())
		{
			auto lightning_pos = position;
			lightning_pos.y -= param.lightning_length;
			lightning_mesh->play(lightning_pos);
			float random = Noise::instance().random_range(0, 360);
			lightning_mesh->set_rotate_quaternion(MeshEffect::AXIS::UP, random);
			lightning_mesh->set_scale({ 0.5f,3.0f,0.5f });
		}
	}


	lightning_mesh->update(elapsed_time);
	//消滅処理+
	life_time += elapsed_time;
	if (life_time > skill_duration) skill_end_flag = true;

}

void LightningRain::render(Camera* camera)
{
	lightning_mesh->render(camera);
}

void LightningRain::lightning_emit(int index_offset, int emit_max_num, float size)
{
}

void LightningRain::debug_gui(string str_id)
{
}
