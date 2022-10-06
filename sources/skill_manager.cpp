#include "skill_manager.h"
#include "device.h"
#include "user.h"
#include "imgui_include.h"
#include "../external/magic_enum/include/magic_enum.hpp"
//コンストラクタ
SkillManager::SkillManager(Graphics& graphics)
{
	sup_slots_ui = make_unique<SkillUI>(graphics, L"./resources/Sprite/UI/Skill/support_skill_icon.png");
	atk_slots_ui = make_unique<SkillUI>(graphics, L"./resources/Sprite/UI/Skill/attack_skill_icon.png");

	support_skills = make_unique<SkillSlot>();
	attack_skills = make_unique<SkillSlot>();
	//身体能力アップスキル
	
	//リジェネスキル
	 
	//拘束スキル
	
	//魔法弾スキル
	
	//槍スキル

	initialize(graphics);
}

//初期化
void SkillManager::initialize(Graphics& graphics)
{
	//サポートスキル枠の初期化
	support_skills->initialize(graphics);

	//攻撃スキル枠の初期化
	attack_skills->initialize(graphics);

	//UI初期設定
	{
		SkillUI::SlotsUi support_ui_init{};
		support_ui_init.center_pos = { 800 ,450 };
		support_ui_init.radius = 0;
		support_ui_init.icon_pos = { 0 ,0 };
		support_ui_init.size = 0.1f;
		support_ui_init.add_ang = -90.0f;
		support_ui_init.expansion_speed = 10.0f;
		support_ui_init.color = { 1,1,1,0 };
		sup_slots_ui->initialize(support_ui_init, static_cast<int>(SupportSkillType::SUP_SKILL_MAX));

		SkillUI::SlotsUi attack_ui_init{};
		attack_ui_init.center_pos = { 800 ,450 };
		attack_ui_init.radius = 0;
		attack_ui_init.icon_pos = { 0 ,0 };
		attack_ui_init.size = 0.1f;
		attack_ui_init.add_ang = -90.0f;
		attack_ui_init.expansion_speed = 10.0f;
		attack_ui_init.color = { 1,1,1,0 };
		atk_slots_ui->initialize(attack_ui_init, static_cast<int>(AttackSkillType::ATK_SKILL_MAX));
	}
}


//更新
void SkillManager::update(Graphics& graphics, float elapsed_time)
{
	//サポートスキル枠,スキルの更新
	support_skills->update(graphics, elapsed_time);

	//攻撃スキル枠,スキルの更新
	attack_skills->update(graphics, elapsed_time);

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

//描画
void SkillManager::render(Graphics& graphics)
{
	//サポートスキルスキルの描画
	support_skills->render(graphics);

	//攻撃スキル枠,スキルの描画
	attack_skills->render(graphics);
}

void SkillManager::ui_render(Graphics& graphics, float elapsed_time)
{
#if USE_IMGUI
	static DirectX::XMFLOAT2 pos1 = { 1000.0f,600.0f };
	static DirectX::XMFLOAT2 pos2 = { 1120.0f,600.0f };
	ImGui::Begin("Skill");
	ImGui::DragFloat2("pos1",&pos1.x);
	ImGui::DragFloat2("pos2",&pos2.x);
	ImGui::End();
#else
	DirectX::XMFLOAT2 pos1 = { 1000.0f,600.0f };
	DirectX::XMFLOAT2 pos2 = { 1120.0f,600.0f };
#endif
	sup_slots_ui->icon_render(graphics);
	sup_slots_ui->selected_skill_icon_render(graphics, pos1);
	atk_slots_ui->icon_render(graphics);
	atk_slots_ui->selected_skill_icon_render(graphics, pos2);
}

//サポートスキル発動

//攻撃スキル発動
void SkillManager::chant_magic_bullet(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 dir)
{
	std::unique_ptr<Skill>skill;
	skill = std::make_unique<MagicBullet>(graphics, launch_pos, launch_pos);
	attack_skills->chant(skill);
}

void SkillManager::chant_spear_sea(Graphics& graphics, DirectX::XMFLOAT3 launch_pos)
{
	std::unique_ptr<Skill>skill;
	skill = std::make_unique<SpearsSea>(graphics, launch_pos);
	attack_skills->chant(skill);
}
//サポートスキルを使用枠にセット
void SkillManager::set_support_skill(int skill_index)
{
	selected_sup_skill_type = static_cast<SupportSkillType>(skill_index);
}
//攻撃スキルを使用枠にセット
void SkillManager::set_attack_skill(int skill_index)
{
	selected_atk_skill_type = static_cast<AttackSkillType>(skill_index);
}


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
	selected_index = ang / slots_ang_size;
	return selected_index;
}

//DebugGui描画
void SkillManager::debug_gui(Graphics& graphics)
{
#if USE_IMGUI

	{
		ImGui::Begin("Skill");

		{
			//現在のスキル表示
			{
				std::string type_name;
				type_name = magic_enum::enum_name<SupportSkillType>(selected_sup_skill_type);
				ImGui::Text(type_name.c_str());
			}
			//スキル選択
			const char* support_slot_item[] = { "physical_up_skill","regeneration_skill","restraint_skill" };
			static int item_current = 0;
			ImGui::Combo("support_slots", &item_current, support_slot_item, IM_ARRAYSIZE(support_slot_item));
			

			
			//発動
			if (ImGui::Button("support_skill_chant"))
			{
				//セット
				set_support_skill(item_current);
				//chant_support_skill(graphics);
			}
		}
		//現在のスキル表示
		{
			std::string type_name;
			type_name = magic_enum::enum_name<AttackSkillType>(selected_atk_skill_type);
			ImGui::Text(type_name.c_str());
		}


		{
			//スキル選択
			const char* attack_slot_item[] = { "magic_bullet_skill","spears_sea_skill" };
			static int item_current2 = 0;
			ImGui::Combo("attack_slots", &item_current2, attack_slot_item, IM_ARRAYSIZE(attack_slot_item));

			
			
			//発動
			if (ImGui::Button("attack_skill_chant"))
			{
				set_attack_skill(item_current2);
				//chant_attack_skill(graphics);
			}
		}
		ImGui::End();
	}

	
		std::string type_name;
		type_name = magic_enum::enum_name<SupportSkillType>(selected_sup_skill_type);
		support_skills->debug_gui(type_name);
	

	
		type_name = magic_enum::enum_name<AttackSkillType>(selected_atk_skill_type);
		attack_skills->debug_gui(type_name);
	

	//UIdebugGUI
	atk_slots_ui->debug_gui("atk_slots_ui");
	sup_slots_ui->debug_gui("sup_slots_ui");
#endif

}
