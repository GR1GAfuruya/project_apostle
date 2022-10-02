#include "skill_manager.h"
#include "device.h"
#include "user.h"
#include "imgui_include.h"
#include "../external/magic_enum/include/magic_enum.hpp"
//コンストラクタ
SkillManager::SkillManager(Graphics& graphics)
{
	sup_slots_ui = make_unique<SkillUI>(graphics);
	atk_slots_ui = make_unique<SkillUI>(graphics);

	//身体能力アップスキル
	physical_up_skill = make_shared<SupportSkillSlot>(graphics, SP_SKILLTYPE::PHYCICAL_UP);
	//リジェネスキル
	regeneration_skill = make_shared<SupportSkillSlot>(graphics, SP_SKILLTYPE::REGENERATE);
	//拘束スキル
	restraint_skill = make_shared<SupportSkillSlot>(graphics, SP_SKILLTYPE::RESTRAINNT);
	//魔法弾スキル
	magic_bullet_skill = make_shared<AttackSkillSlot>(graphics, AT_SKILLTYPE::MAGICBULLET);
	//槍スキル
	spears_sea_skill = make_shared<AttackSkillSlot>(graphics, AT_SKILLTYPE::SPEARS_SEA);
	initialize(graphics);
}

//初期化
void SkillManager::initialize(Graphics& graphics)
{
	//サポートスキルの枠
	support_skill_slots = { physical_up_skill,regeneration_skill,restraint_skill};
	//攻撃スキルの枠
	attack_skill_slots = { magic_bullet_skill,spears_sea_skill };

	//サポートスキル枠の初期化
	for (auto& s : support_skill_slots)
	{
		s->initialize(graphics);
	}

	//攻撃スキル枠の初期化
	for (auto& s : attack_skill_slots)
	{
		s->initialize(graphics);
	}
	support_skill = regeneration_skill.get();
	attack_skill = magic_bullet_skill.get();

	SkillUI::SlotsUi support_ui_init;
	support_ui_init.center_pos = { 800 ,450 };
	support_ui_init.radius = 0;
	support_ui_init.icon_pos = { 0 ,0 };
	support_ui_init.size = 0.1f;
	support_ui_init.add_ang = -90.0f;
	support_ui_init.expansion_speed = 10.0f;
	support_ui_init.color = { 1,1,1,0 };
	sup_slots_ui->initialize(support_ui_init);

	SkillUI::SlotsUi attack_ui_init;
	attack_ui_init.center_pos = { 800 ,450 };
	attack_ui_init.radius = 0;
	attack_ui_init.icon_pos = { 0 ,0 };
	attack_ui_init.size = 0.1f;
	attack_ui_init.add_ang = -90.0f;
	attack_ui_init.expansion_speed = 10.0f;
	attack_ui_init.color = { 1,0,0,0 };
	atk_slots_ui->initialize(attack_ui_init);
}


//更新
void SkillManager::update(Graphics& graphics, float elapsed_time)
{
	//サポートスキル枠の更新
	for (auto& s : support_skill_slots)
	{
		s->update(graphics, elapsed_time);
	}

	//攻撃スキル枠の更新
	for (auto& s : attack_skill_slots)
	{
		s->update(graphics, elapsed_time);
	}

	//UIアップデート
	sup_slots_ui->update(graphics, elapsed_time);
	atk_slots_ui->update(graphics, elapsed_time);

	//スキル選択更新
	GamePad* game_pad = &Device::instance().get_game_pad();
	//サポートスキル選択状態
	if (game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
	{
		sup_slots_ui->set_skill_select(true);
		atk_slots_ui->set_skill_select(false);
	}
	//ボタンを放したら解除
	else if(game_pad->get_button_up() & GamePad::BTN_RIGHT_SHOULDER)
	{
		sup_slots_ui->set_skill_select(false);
	}
	//攻撃スキル選択状態
	if (game_pad->get_button_down() & GamePad::BTN_LEFT_SHOULDER)
	{
		atk_slots_ui->set_skill_select(true);
		sup_slots_ui->set_skill_select(false);

	}
	//ボタンを放したら解除
	else if(game_pad->get_button_up() & GamePad::BTN_LEFT_SHOULDER)
	{
		atk_slots_ui->set_skill_select(false);
	}
	if (game_pad->get_button() & GamePad::BTN_RIGHT_SHOULDER)
	{
		//入力情報を取得
		float ax = game_pad->get_axis_RX();
		float ay = game_pad->get_axis_RY();
		DirectX::XMFLOAT2 vec = { ax,ay };
		//入力されていない場合は抜ける
		if (Math::Length(vec) == 0) return;

		set_support_skill(select_skill_slot(vec));
	}

	if (game_pad->get_button() & GamePad::BTN_LEFT_SHOULDER)
	{
		//入力情報を取得
		float ax = game_pad->get_axis_RX();
		float ay = game_pad->get_axis_RY();
		DirectX::XMFLOAT2 vec = { ax,ay };
		//入力されていない場合は抜ける
		if (Math::Length(vec) == 0) return;

		set_attack_skill(select_skill_slot(vec));
	}
	

	
}

//描画
void SkillManager::render(Graphics& graphics)
{
	//サポートスキル枠の描画
	for (auto& s : support_skill_slots)
	{
		s->render(graphics);
	}
	//攻撃スキル枠の描画
	for (auto& s : attack_skill_slots)
	{
		s->render(graphics);
	}
}

void SkillManager::ui_render(Graphics& graphics, float elapsed_time)
{

	sup_slots_ui->icon_render(graphics);
	atk_slots_ui->icon_render(graphics);
}

//サポートスキル発動
void SkillManager::chant_support_skill(Graphics& graphics)
{
	_ASSERT_EXPR(support_skill != nullptr, L"support_skill is null");

	support_skill->chant(graphics);
}

//攻撃スキル発動
void SkillManager::chant_attack_skill(Graphics& graphics)
{
	_ASSERT_EXPR(attack_skill != nullptr, L"attack_skill is null");

	attack_skill->chant(graphics);
}

//サポートスキルを使用枠にセット
void SkillManager::set_support_skill(int skill_index)
{
	skill_index = Math::clamp(skill_index, 0, 2);
	support_skill = support_skill_slots.at(skill_index).get();
}

//攻撃スキルを使用枠にセット
void SkillManager::set_attack_skill(int skill_index)
{
	skill_index = Math::clamp(skill_index, 0, 1);
	attack_skill = attack_skill_slots.at(skill_index).get();
}

int SkillManager::select_skill_slot(DirectX::XMFLOAT2 stick_vec)
{
	//基準軸
	DirectX::XMFLOAT2 up = { 0.00001f,1.0f };
	//正規化
	stick_vec = Math::Normalize(stick_vec);
	//スティックのさす方向と基準軸間の角度を求める
	float dot = Math::Dot(up, stick_vec);
	dot = acosf(dot);
	float deglee_dot = DirectX::XMConvertToDegrees(dot);
	//x値がマイナスの場合補正
	float add_angle = 180 - deglee_dot;
	float ang = deglee_dot;
	if (stick_vec.x < 0) ang = 180 + add_angle;
	//スロット一つ分の枠の大きさを決定
	float slots_ang_size = 360.0f / ((support_skill_slots.size() + 1) * 2);
	int selected_index;
	//スロット番号を決定
	selected_index = ang / slots_ang_size;
	//不正な値が入らないようクランプ
	
	return selected_index;
}

//DebugGui描画
void SkillManager::debug_gui(Graphics& graphics)
{
#if USE_IMGUI

	{
		ImGui::Begin("Skill");

		{
			//スキル選択
			const char* support_slot_item[] = { "physical_up_skill","regeneration_skill","restraint_skill" };
			static int item_current = 0;
			ImGui::Combo("support_slots", &item_current, support_slot_item, IM_ARRAYSIZE(support_slot_item));
			//セット

			set_support_skill(item_current);

			
			//発動
			if (ImGui::Button("support_skill_chant"))
			{
				chant_support_skill(graphics);
			}
		}

		{
			//スキル選択
			const char* attack_slot_item[] = { "magic_bullet_skill","spears_sea_skill" };
			static int item_current2 = 0;
			ImGui::Combo("attack_slots", &item_current2, attack_slot_item, IM_ARRAYSIZE(attack_slot_item));

			set_attack_skill(item_current2);
			
			//発動
			if (ImGui::Button("attack_skill_chant"))
			{
				chant_attack_skill(graphics);
			}
		}
		ImGui::End();
	}

	for (auto& s : support_skill_slots)
	{
		std::string type_name;
		type_name = magic_enum::enum_name<SupportSkillSlot::SupportSkillType>(s->skill_type);
		s->debug_gui(type_name);
	}

	for (auto& s : attack_skill_slots)
	{
		std::string type_name;
		type_name = magic_enum::enum_name<AttackSkillSlot::AttackSkillType>(s->skill_type);
		s->debug_gui(type_name);
	}
#endif

}
