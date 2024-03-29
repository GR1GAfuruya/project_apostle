#include "boss.h"
#include "user.h"
#include "Operators.h"



#include <filesystem>
#include <fstream>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>

//==============================================================
// 
// 初期化
// 
//==============================================================
void Boss::initialize()
{
	load_data_file();
	chara_param = param.chara_init_param;

	transition_idle_state();
	position = { -24.0f, -7.0f,7.0f };
	scale.x = scale.y = scale.z = 0.08f;
	//体力初期化
	health = param.chara_init_param.max_health;
	//速度初期化
	velocity = { 0.0f, 0.0f, 0.0f };
	//エフェクト初期化
	attack_skill_2->stop();

	damaged_function = [=](int damage, float invincible, WINCE_TYPE type)->bool {return apply_damage(damage, invincible, type); };
	sickle_hand = model->get_bone_by_name("Bip01-R-ForeTwist");
	sickle_hand_colide.radius = 8.0f;
	vs_wall_ray_power = 10.0f;
	state_duration = 2.0f;
	boss_body_collision.capsule.start = position;
	boss_body_collision.capsule.radius = 10;
	boss_body_collision.height = 25;
	boss_body_collision.capsule.end = boss_body_collision.capsule.start;
	boss_body_collision.capsule.end.y = boss_body_collision.capsule.start.y + boss_body_collision.height;
}
//==============================================================
// 
// コンストラクタ
// 
//==============================================================
Boss::Boss()
{
	Graphics& graphics = Graphics::instance();
	model = make_unique<SkeletalMesh>(graphics.get_device().Get(), "./resources/Model/Boss/LordHell.fbx", 60.0f);
	attack_skill_1 = make_unique<BossAttackSkill1>();
	attack_skill_2 = make_unique<ChargeAttack>();
	attack_skill_3 = make_unique<BossAttackSkill3>();
	ui = make_unique<BossUi>();

	initialize();

}
//==============================================================
// 
//更新処理
// 
//==============================================================
void Boss::update(float elapsed_time, Camera* camera)
{
#if _DEBUG
	if (!is_update) return;
#endif

	(this->*act_update)(elapsed_time);
	model->update_animation(elapsed_time);
	DirectX::XMFLOAT4X4 sickle_bone_mat;
	model->fech_by_bone(transform, sickle_hand, sickle_hand_colide.start, &sickle_bone_mat);

	//スキル１のアップデート
	attack_skill_1->update(elapsed_time, camera, sickle_hand_colide.start, Math::get_posture_right(sickle_bone_mat));
	//スキル2のアップデート
	attack_skill_2->update(elapsed_time, camera);
	attack_skill_2->set_target_pos(target_pos);
	//スキル3のアップデート
	attack_skill_3->update(elapsed_time, camera, sickle_hand_colide.start, Math::get_posture_right(sickle_bone_mat));

	//bodyの攻撃用当たり判定
	boss_body_collision.capsule.start = position;
	boss_body_collision.capsule.end = boss_body_collision.capsule.start;
	boss_body_collision.capsule.end.y = boss_body_collision.capsule.start.y + boss_body_collision.height;


	//腕の当たり判定設定
	sickle_hand_colide.end = sickle_hand_colide.start + Math::vector_scale(Math::get_posture_right(sickle_bone_mat), 5.0f);

	//無敵時間更新
	update_invicible_timer(elapsed_time);
	//UI更新
	ui->update(elapsed_time);
}
//==============================================================
// 
//描画処理（ディファード）
// 
//==============================================================
void Boss::render_d(float elapsed_time, Camera* camera)
{
	Graphics& graphics = Graphics::instance();
#if _DEBUG
	if (!is_render) return;
#endif

	transform = Math::calc_world_matrix(scale, orientation, position);
	graphics.shader->render(graphics.get_dc().Get(), model.get(), transform);
}
//==============================================================
// 
//描画処理（フォワード）
// 
//==============================================================
void Boss::render_f(float elapsed_time, Camera* camera)
{
	Graphics& graphics = Graphics::instance();
	attack_skill_1->render(camera);
	attack_skill_2->render(camera);
	attack_skill_3->render(camera);
	debug_gui();

}
//==============================================================
// 
//描画処理（シャドウ）
// 
//==============================================================
void Boss::render_s(float elapsed_time, Camera* camera)
{
	Graphics& graphics = Graphics::instance();
	graphics.shader->render(graphics.get_dc().Get(), model.get(), transform);

}
//==============================================================
// 
//描画処理（UI）
// 
//==============================================================
void Boss::render_ui(float elapsed_time)
{
	Graphics& graphics = Graphics::instance();
	ui->set_percent(get_hp_percent());
	ui->render(graphics.get_dc().Get());
}
//==============================================================
// 
//死亡処理
// 
//==============================================================
void Boss::on_dead()
{
	transition_air_bone_state();
}
//==============================================================
// 
//ダメージを受けた際の処理
// 
//==============================================================
void Boss::on_damaged(WINCE_TYPE type)
{
	/*ステートがダメージを受けているか攻撃している途中
	 あるいは怯み処理を行わない攻撃タイプの時は処理を飛ばす*/
	if (state == State::DAMAGE || state == State::ATTACK || type == WINCE_TYPE::NONE)
	{
		return;
	}
	transition_damage_state();
}
void Boss::load_data_file()
{
	// Jsonファイルから値を取得
	std::filesystem::path path = file_path;
	path.replace_extension(".json");
	if (std::filesystem::exists(path.c_str()))
	{
		std::ifstream ifs;
		ifs.open(path);
		if (ifs)
		{
			cereal::JSONInputArchive o_archive(ifs);
			o_archive(param);
		}
	}
}
void Boss::save_data_file()
{
	//ベースクラスの初期化パラメーター情報を更新
	// Jsonファイルから値を取得
	std::filesystem::path path = file_path;
	path.replace_extension(".json");
	std::ofstream ifs;
	ifs.open(path);
	if (ifs)
	{
		cereal::JSONOutputArchive o_archive(ifs);
		o_archive(param);
	}
}
;
//==============================================================
// 
//デバッグGUI表示
// 
//==============================================================
void Boss::debug_gui()
{

#if USE_IMGUI
	imgui_menu_bar("Charactor", "boss", display_imgui);
	if (display_imgui)
	{

		if (ImGui::Begin("Boss", nullptr, ImGuiWindowFlags_None))
		{
#if _DEBUG
			ImGui::Checkbox("is_update", &is_update);
			ImGui::Separator();
			ImGui::Checkbox("is_render", &is_render);
#endif
			if (ImGui::CollapsingHeader("Skill", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::Button("skill_1"))
				{
					transition_skill_1_state();
				}
				if (ImGui::Button("skill_2"))
				{
					transition_skill_2_start_state();
				}
				if (ImGui::Button("skill_3"))
				{
					transition_skill_3_state();
				}
			}
			if (ImGui::Button("load"))
			{
				load_data_file();
			}
			ImGui::Separator();
			if (ImGui::Button("save"))
			{
				save_data_file();
			}
			//トランスフォーム
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				//位置
				ImGui::DragFloat3("Position", &position.x);
				//回転				
				ImGui::DragFloat3("scale:", &scale.x);
				//速度
				ImGui::DragFloat3("velocity:", &velocity.x);
			}
			ImGui::DragInt("hp", &health);
			ImGui::DragFloat("height", &chara_param.height);
			ImGui::DragFloat("turnspeed", &chara_param.turn_speed, 0.1f);
			ImGui::DragFloat("boss_collision.radius", &boss_body_collision.capsule.radius, 0.1f);
			ImGui::DragFloat("boss_collision.height", &boss_body_collision.height, 0.1f);
			ImGui::DragFloat("sickle_.radius", &sickle_hand_colide.radius, 1);
		}
		ImGui::End();
	}
	attack_skill_1->debug_gui("");
	attack_skill_2->debug_gui("");
#endif
}
//==============================================================
// 
//ボスの攻撃とプレイヤーの当たり判定処理
// 
//==============================================================
void Boss::calc_attack_vs_player(DirectX::XMFLOAT3 player_cap_start, DirectX::XMFLOAT3 player_cap_end, float colider_radius, AddDamageFunc damaged_func)
{
	if (sickle_attack_param.is_attack)
	{

		if (Collision::capsule_vs_capsule(player_cap_start, player_cap_end, colider_radius,
			sickle_hand_colide.start, sickle_hand_colide.end, sickle_hand_colide.radius))
		{
			sickle_attack_param.power = 5;
			sickle_attack_param.invinsible_time = 0.55f;
			damaged_func(sickle_attack_param.power, sickle_attack_param.invinsible_time, WINCE_TYPE::NONE);
		}
	}

	attack_skill_1->calc_vs_player(player_cap_start, player_cap_end, colider_radius, damaged_func);
	attack_skill_2->calc_vs_player(player_cap_start, player_cap_end, colider_radius, damaged_func);
	attack_skill_3->calc_vs_player(player_cap_start, player_cap_end, colider_radius, damaged_func);
}

