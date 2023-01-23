#include "skill_manager.h"
#include "device.h"
#include "user.h"
#include "imgui_include.h"
#include "../external/magic_enum/include/magic_enum.hpp"
//==============================================================
// 
// コンストラクタ
// 
//==============================================================
SkillManager::SkillManager(Graphics& graphics)
{
	sup_slots_ui = make_unique<SkillUI>(graphics, L"./resources/Sprite/UI/Skill/support_skill_icon.png");
	atk_slots_ui = make_unique<SkillUI>(graphics, L"./resources/Sprite/UI/Skill/attack_skill_icon.png");

	//身体能力アップスキル
	physical_up = make_unique<PhysicalUpLauncher>(graphics);
	
	//リジェネスキル
	regenerate = make_unique<RegenerateLauncher>(graphics);
	 
	//拘束スキル
	restraint = make_unique<RestraintLauncher>(graphics);
	
	//魔法弾スキル
	magick_bullet = make_unique<MagicBulletLauncher>(graphics);
	
	//槍スキル
	spear_sea = make_unique<SpearSeaLauncher>(graphics);

	//斬撃波スキル
	slash_wave = make_unique<SlashWaveLauncher>(graphics);
	
	//雷雨スキル
	lightning_rain = make_unique<LightningRainLauncher>(graphics);
	//初期スキル設定
	selected_sup_skill = physical_up.get();
	selected_atk_skill = magick_bullet.get();

	//リストに追加
	sup_skill_list = { physical_up, regenerate,restraint };
	atk_skill_list = { magick_bullet, spear_sea, slash_wave, lightning_rain };
	//パラメーター初期化
	initialize(graphics);
	
}


//==============================================================
// 
// 初期化
// 
//==============================================================
void SkillManager::initialize(Graphics& graphics)
{
	//スキルの初期化
	{
		for (auto& s : sup_skill_list)
		{
			s->initialize(graphics);
		}
		for (auto& s : atk_skill_list)
		{
			s->initialize(graphics);
		}
	}
	//UI初期設定
	{
		SkillUI::SlotsUi support_ui_init{};
		support_ui_init.center_pos = { 800 ,450 };
		support_ui_init.radius = 0;
		support_ui_init.icon_pos = { 0 ,0 };
		support_ui_init.size = 0.4f;
		support_ui_init.add_ang = -90.0f;
		support_ui_init.expansion_speed = 10.0f;
		support_ui_init.color = { 1,1,1,0 };
		support_ui_init.cool_time_gauge_pos = { 1060.0f, 650.0f };
		sup_slots_ui->initialize(support_ui_init, static_cast<int>(SupportSkillType::SUP_SKILL_MAX));

		SkillUI::SlotsUi attack_ui_init{};
		attack_ui_init.center_pos = { 800 ,450 };
		attack_ui_init.radius = 0;
		attack_ui_init.icon_pos = { 0 ,0 };
		attack_ui_init.size = 0.4f;
		attack_ui_init.add_ang = -90.0f;
		attack_ui_init.expansion_speed = 10.0f;
		attack_ui_init.color = { 1,1,1,0 };
		attack_ui_init.cool_time_gauge_pos = { 1180.0f, 650.0f };
		atk_slots_ui->initialize(attack_ui_init, static_cast<int>(AttackSkillType::ATK_SKILL_MAX));
	}


}


//==============================================================
// 
//更新処理
// 
//==============================================================
void SkillManager::update(Graphics& graphics, float elapsed_time)
{
	//サポートスキルの更新
	for(auto& s : sup_skill_list)
	{
		s->update(graphics, elapsed_time);
	}
	//攻撃スキルの更新
	for (auto& s : atk_skill_list)
	{
		s->update(graphics, elapsed_time);
	}
	//UIアップデート
	sup_slots_ui->update(graphics, elapsed_time);
	sup_slots_ui->set_selected_skill_index(static_cast<int>(selected_sup_skill_type));
	atk_slots_ui->update(graphics, elapsed_time);
	atk_slots_ui->set_selected_skill_index(static_cast<int>(selected_atk_skill_type));

	//スキル選択更新
	GamePad* game_pad = &Device::instance().get_game_pad();
	//サポートスキル選択状態
	if (game_pad->get_button_down() & GamePad::BTN_LEFT_SHOULDER)
	{
		//サポートスキルスロットのUIを展開
		sup_slots_ui->set_skill_select(true);
		//攻撃スキルスロットのUIを閉じる
		atk_slots_ui->set_skill_select(false);
		is_selecting_support_skill = true;
	}
	//ボタンを放したら解除
	else if(game_pad->get_button_up() & GamePad::BTN_LEFT_SHOULDER)
	{
		//サポートスキルスロットのUIを閉じる
		sup_slots_ui->set_skill_select(false);
		is_selecting_support_skill = false;
	}

	//攻撃スキル選択状態
	if (game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
	{
		//攻撃スキルスロットのUIを展開
		atk_slots_ui->set_skill_select(true);
		//サポートスキルスロットのUIを閉じる
		sup_slots_ui->set_skill_select(false);
		is_selecting_attack_skill = true;

	}
	//ボタンを放したら解除
	else if(game_pad->get_button_up() & GamePad::BTN_RIGHT_SHOULDER)
	{
		atk_slots_ui->set_skill_select(false);
		is_selecting_attack_skill = false;
	}

	//スキルを選んでセットする処理
	{
		if (game_pad->get_button() & GamePad::BTN_LEFT_SHOULDER)
		{
			//サポートスキル
			//入力情報を取得
			float ax = game_pad->get_axis_RX();
			float ay = game_pad->get_axis_RY();
			DirectX::XMFLOAT2 vec = { ax,ay };
			//入力されていない場合は抜ける
			if (Math::Length(vec) == 0) return;

			set_support_skill(select_skill_slot(vec, static_cast<int>(SupportSkillType::SUP_SKILL_MAX)));
		}

		//攻撃スキルスキル
		if (game_pad->get_button() & GamePad::BTN_RIGHT_SHOULDER)
		{
			//入力情報を取得
			float ax = game_pad->get_axis_RX();
			float ay = game_pad->get_axis_RY();
			DirectX::XMFLOAT2 vec = { ax,ay };
			//入力されていない場合は抜ける
			if (Math::Length(vec) == 0) return;

			set_attack_skill(select_skill_slot(vec, static_cast<int>(AttackSkillType::ATK_SKILL_MAX)));
		}
	}

	
}

//==============================================================
// 
//描画処理
// 
//==============================================================
void SkillManager::render(Graphics& graphics)
{
	//スキルの描画
	{
		for (auto& s : sup_skill_list)
		{
			s->render(graphics);
		}
		for (auto& s : atk_skill_list)
		{
			s->render(graphics);
		}
	}
}
//==============================================================
// 
//描画処理（UI）
// 
//==============================================================
void SkillManager::ui_render(Graphics& graphics, float elapsed_time)
{
#if USE_IMGUI
	static DirectX::XMFLOAT2 pos1 = { 1000.0f,600.0f };
	static DirectX::XMFLOAT2 pos2 = { 1120.0f,600.0f };
	if (display_imgui)
	{
		ImGui::Begin("Skill");
		ImGui::DragFloat2("pos1", &pos1.x);
		ImGui::DragFloat2("pos2", &pos2.x);
		ImGui::End();
	}
#else
	DirectX::XMFLOAT2 pos1 = { 1000.0f,600.0f };
	DirectX::XMFLOAT2 pos2 = { 1120.0f,600.0f };
#endif
	sup_slots_ui->icon_render(graphics);
	sup_slots_ui->selected_skill_icon_render(graphics, pos1);
	atk_slots_ui->icon_render(graphics);
	atk_slots_ui->selected_skill_icon_render(graphics, pos2);

	if (selected_sup_skill->get_cool_time_percent() > 0.0f)
	{
		sup_slots_ui->cool_time_render(graphics, elapsed_time, selected_sup_skill->get_cool_time_percent());
	}
	if (selected_atk_skill->get_cool_time_percent() > 0.0f)
	{
		atk_slots_ui->cool_time_render(graphics, elapsed_time, selected_atk_skill->get_cool_time_percent());
	}

}

//==============================================================
// 
//PhycicalUp発動
// 
//==============================================================
bool SkillManager::chant_physical_up(Graphics& graphics, DirectX::XMFLOAT3* launch_pos, float* add_run_speed, float* add_jump_speed)
{
	return physical_up->chant(graphics, launch_pos, add_run_speed, add_jump_speed);
}
//==============================================================
// 
//Regenerate発動
// 
//==============================================================
bool SkillManager::chant_regenerate(Graphics& graphics, DirectX::XMFLOAT3* launch_pos, int* health, const int max_health)
{
	return regenerate->chant(graphics, launch_pos, health, max_health);
}
//==============================================================
// 
//Restraint発動
// 
//==============================================================
bool SkillManager::chant_restraint(Graphics& graphics, DirectX::XMFLOAT3* target_pos, float* down_speed)
{
	return restraint->chant(graphics, target_pos, target_pos);
}
//==============================================================
// 
//MagickBullet発動
// 
//==============================================================
bool SkillManager::chant_magic_bullet(Graphics& graphics, DirectX::XMFLOAT3* launch_pos, DirectX::XMFLOAT3* dir)
{
	return magick_bullet->chant(graphics, launch_pos, dir);
}
//==============================================================
// 
//SpearSea発動
// 
//==============================================================
bool SkillManager::chant_spear_sea(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos)
{
	return spear_sea->chant(graphics, launch_pos, target_pos);
}

bool SkillManager::chant_slash_wave(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3* dir)
{
	return slash_wave->chant(graphics, launch_pos, dir);
}

bool SkillManager::chant_lightning_rain(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos)
{
	return lightning_rain->chant(graphics, launch_pos, target_pos);
}

//==============================================================
// 
//クールタイム短縮
// 
//==============================================================
void SkillManager::cool_time_reduction()
{
	//サポートスキルのクールタイム短縮
	for (auto& s : sup_skill_list)
	{
		s->cool_time_reduction();
	}
	//攻撃スキルのクールタイム短縮
	for (auto& s : atk_skill_list)
	{
		s->cool_time_reduction();
	}

}


//==============================================================
// 
//魔法弾と敵の当たり判定
// 
//==============================================================
void SkillManager::judge_magic_bullet_vs_enemy(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera)
{
	magick_bullet->skill_object_hit_judgment(object_colider, damaged_func, camera);
}

void SkillManager::judge_spear_sea_vs_enemy(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera)
{
	spear_sea->skill_object_hit_judgment(object_colider, damaged_func, camera);
}

//==============================================================
// 
//サポートスキルを使用枠にセット
// 
//==============================================================
void SkillManager::set_support_skill(int skill_index)
{
	selected_sup_skill_type = static_cast<SupportSkillType>(skill_index);
	selected_sup_skill = sup_skill_list.at(skill_index).get();
}
//==============================================================
// 
//攻撃スキルを使用枠にセット
// 
//==============================================================
void SkillManager::set_attack_skill(int skill_index)
{
	selected_atk_skill_type = static_cast<AttackSkillType>(skill_index);
	selected_atk_skill = atk_skill_list.at(skill_index).get();
}

//==============================================================
// 
//コントローラーの右スティックでスキルを選択
// 
//==============================================================
int SkillManager::select_skill_slot(DirectX::XMFLOAT2 stick_vec, int slot_num)
{
	//スロット一つ分の枠の大きさを決定
	float slots_ang_size = 360.0f / slot_num;
	//基準軸 操作しやすいようにスロット一つ分の枠の大きさ÷2分左に傾ける
	float correction_standard_ang = -DirectX::XMConvertToRadians(slots_ang_size / 2);
	DirectX::XMFLOAT2 standard_axis = { sinf(correction_standard_ang), cosf(correction_standard_ang)};
	//正規化
	stick_vec = Math::Normalize(stick_vec);
	//スティックのさす方向と基準軸間の角度を求める
	float dot = Math::Dot(standard_axis, stick_vec);
	dot = acosf(dot);
	float deglee_dot = DirectX::XMConvertToDegrees(dot);
	//Dotが180度を上回ったときに補正をかけて360度に対応できるようにする
	float ang = deglee_dot;
	float cross{ stick_vec.x * standard_axis.y - stick_vec.y * standard_axis.x };
	if (cross < 0) ang = 360.0f - ang;
	//スロット番号を決定
	int selected_index;
	selected_index = static_cast<int>(ang / slots_ang_size);
	return selected_index;
}

//==============================================================
// 
//デバッグGUI表示
// 
//==============================================================
void SkillManager::debug_gui(Graphics& graphics)
{
#if USE_IMGUI
	imgui_menu_bar("Skill", "SkillManager", display_imgui);
	if (display_imgui)
	{

		ImGui::Begin("Skill");

		{
			//現在のスキル表示
			{
				std::string type_name;
				type_name = magic_enum::enum_name<SupportSkillType>(selected_sup_skill_type);
				ImGui::Text(type_name.c_str());
				ImGui::Separator();

			}
			//スキル選択
			const char* support_slot_item[] = { "physical_up_skill","regeneration_skill","restraint_skill" };
			static int item_current = 0;
			ImGui::Separator();
			ImGui::Combo("support_slots", &item_current, support_slot_item, IM_ARRAYSIZE(support_slot_item));
			
			
			//発動
			if (ImGui::Button("set_support_skill"))
			{
				//セット
				set_support_skill(item_current);
			}
			ImGui::Separator();
		}
		//現在のスキル表示
		{
			std::string type_name;
			type_name = magic_enum::enum_name<AttackSkillType>(selected_atk_skill_type);
			ImGui::Text(type_name.c_str());
			ImGui::Separator();

		}


		{
			//スキル選択
			const char* attack_slot_item[] = { "magic_bullet_skill","spears_sea_skill" };
			static int item_current2 = 0;
			ImGui::Separator();
			ImGui::Combo("attack_slots", &item_current2, attack_slot_item, IM_ARRAYSIZE(attack_slot_item));

			
			
			//発動
			if (ImGui::Button("set_attack_skill"))
			{
				set_attack_skill(item_current2);
			}
				ImGui::Separator();
		}
		ImGui::End();
	}

	
	//身体能力アップスキル
	physical_up->debug_gui();
	//リジェネスキル
	regenerate->debug_gui();
	//拘束スキル
	restraint->debug_gui();
	//魔法弾スキル
	magick_bullet->debug_gui();
	//槍スキル
	spear_sea->debug_gui();
	

	//UIdebugGUI
	atk_slots_ui->debug_gui("atk_slots_ui");
	sup_slots_ui->debug_gui("sup_slots_ui");
#endif

}
