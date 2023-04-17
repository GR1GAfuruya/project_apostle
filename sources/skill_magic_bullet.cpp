#include "skill_magic_bullet.h"
#include "Operators.h"
#include "light_manager.h"
#include "noise.h"
//==============================================================
// 
//コンストラクタ
// 
//==============================================================
MagicBullet::MagicBullet(Graphics& graphics, DirectX::XMFLOAT3* init_pos, DirectX::XMFLOAT3* dir, PublicParam init_param)
{
	initialize(graphics);

	//各初期化パラメーター設定
	acceleration = init_param.acceleration;
	attack_colider.radius = init_param.collider_radius;
	skill_duration = 1;
	launch_pos.reset(init_pos);
	target_dir.reset(dir);
	launch_bullet = false;
	//エフェクト
	{
		main_effect = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/eff_spear.fbx");
		main_effect->set_material(MaterialManager::instance().mat_fire_distortion.get());
		main_effect->set_init_scale(0.5f);
		main_effect->set_init_life_duration(0.5f);
		main_effect->set_init_color({ 1.0f,0.8f,5.5f,1.0f });
	}

	{
		lightning_disk_effect = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/disk.fbx");
		lightning_disk_effect->set_material(MaterialManager::instance().mat_fire_distortion.get());
		lightning_disk_effect->set_init_scale(1.0f);
		lightning_disk_effect->set_init_life_duration(0.5f);
		lightning_disk_effect->set_init_color({ 1.0f,0.8f,5.5f,1.0f });
	}

	for (auto& l : lightning_effect)
	{
		l = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/lightning.fbx");
		l->set_material(MaterialManager::instance().mat_lightning.get());
		l->set_init_scale(1.5f);
		l->set_init_life_duration(0.5f);
		l->set_init_color({ 1.0f,0.8f,5.5f,1.0f });
	}

	//ライト生成
	spear_light = make_shared<PointLight>(graphics, *launch_pos, 30.0f, DirectX::XMFLOAT3(1.0f, 0.8f, 5.5f));
	LightManager::instance().register_light("MagicBullet", spear_light);

};

//==============================================================
// 
//デストラクタ
// 
//==============================================================
MagicBullet::~MagicBullet()
 {
	launch_pos.release();
	target_dir.release();
	//ライト消去
	 LightManager::instance().delete_light(spear_light->name);
 }
 //==============================================================
 // 
 //初期化
 // 
 //==============================================================
 void MagicBullet::initialize(Graphics& graphics)
{
	life_time = 0.5f;
	collision_type = CollisionType::SPHERE;

}
//==============================================================
// 
//更新
// 
//==============================================================
 void MagicBullet::update(Graphics& graphics, float elapsed_time)
 {
	 //弾射出
	 if (!launch_bullet)
	 {
		 //アニメーションフレームが特定の値に達したら弾射出
		 if (life_time > 0.8f)
		 {
			 main_effect->play(*launch_pos);
			 main_effect->rotate_base_axis(MeshEffect::AXIS::FORWARD, Math::Normalize(*target_dir));

			 lightning_disk_effect->play(*launch_pos);
			 lightning_disk_effect->rotate_base_axis(MeshEffect::AXIS::UP, Math::Normalize(*target_dir));
			 velocity = acceleration * Math::Normalize(*target_dir);


			 position = *launch_pos;

			 launch_bullet = true;
		 }
		 else if(life_time > 0.7f)
		 {

			 for (auto& l : lightning_effect)
			 {
				 if (!l->get_active())
				 {
					 l->play(*launch_pos);
					 l->rotate_base_axis(MeshEffect::AXIS::UP, Math::Normalize(*target_dir));
				 }
				 float random = Noise::instance().random_range(0, 360);
				 l->set_rotate_quaternion(MeshEffect::AXIS::UP, random);

			 }
		 }


	 }
	 else
	 {
		 //
		 velocity *= 1.05f;
		 position += velocity * elapsed_time;
		 attack_colider.start = position;
		 //エフェクト更新
		 main_effect->set_position(position);

		 //ライト位置更新
		 spear_light->set_position(position);

	 }



	 main_effect->update(graphics, elapsed_time);
	 main_effect->set_is_loop(true);

	 for (auto& l : lightning_effect)
	 {
		 l->update(graphics, elapsed_time);
	 }

	 lightning_disk_effect->update(graphics, elapsed_time);
	 lightning_disk_effect->set_scale(lerp(lightning_disk_effect->get_scale().x, 0.0f, 1.0f * elapsed_time));


	 //消滅処理
	 life_time += elapsed_time;
	 if (life_time > skill_duration) skill_end_flag = true;
 };
//==============================================================
// 
//描画
// 
//==============================================================
 void MagicBullet::render(Graphics& graphics, Camera* camera)
 {
	 if (is_hit)
	 {
	 }
		 main_effect->render(graphics, camera);

	 for (auto& l : lightning_effect)
	 {
		 l->render(graphics, camera);
	 }
	 lightning_disk_effect->render(graphics, camera);


 }
//==============================================================
// 
//デバッグGUI
// 
//==============================================================
void MagicBullet::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "MagicBullet:" + str_id + "Param";
	ImGui::Begin("MagicBullet");
	ImGui::PushID(str_id.c_str());
	/*これより下にパラメーター記述*/
	ImGui::BulletText(name.c_str());
	ImGui::DragFloat3("position", &position.x);
	ImGui::DragFloat3("velocity", &velocity.x);
	ImGui::DragFloat("life_time", &life_time);
	

	/*これより上にパラメーター記述*/
	ImGui::PopID();
	ImGui::End();

#endif

}
