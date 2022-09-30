#include "skill_manager.h"
#include "user.h"
#include "imgui_include.h"
#include "../external/magic_enum/include/magic_enum.hpp"
//コンストラクタ
SkillManager::SkillManager(Graphics& graphics)
{
	initialize(graphics);
	skill_slot_icon = make_unique<SpriteBatch>(graphics.get_device().Get(), L"./resources/Sprite/skill_icon.png", 10);
}

//初期化
void SkillManager::initialize(Graphics& graphics)
{
	//身体能力アップスキル
	physical_up_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::PHYCICAL_UP);
	//リジェネスキル
	regeneration_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::REGENERATE);
	//拘束スキル
	restraint_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::RESTRAINNT);
	//魔法弾スキル
	magic_bullet_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::MAGICBULLET);
	//槍スキル
	spears_sea_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::SPEARS_SEA);

	//サポートスキルの枠
	support_skill_slots = { physical_up_skill,regeneration_skill,restraint_skill};
	//攻撃スキルの枠
	attack_skill_slots = { magic_bullet_skill,spears_sea_skill };
	//regeneration_skill = make_shared<SkillSlot>(graphics, SKILLTYPE::REGENERATE);

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

	DirectX::XMFLOAT2 icon_pos = { 0 ,0 };
#if USE_IMGUI
	static DirectX::XMFLOAT2 center_pos = { 860 ,430 };
	static float radius = 150;
	static float add_ang = 40;
	{

		ImGui::Begin("Skill");
		ImGui::DragFloat2("center_pos", &center_pos.x);
		ImGui::DragFloat("radius", &radius);
		ImGui::DragFloat("add_ang", &add_ang);
		if (ImGui::Button("reset"))
		{
		}
			radius = 0;
		ImGui::End();
	}
#else
	 const DirectX::XMFLOAT2 center_pos = { 860 ,430 };
	 const float radius = 150;
	 const float add_ang = 40;
#endif
	const int TILE_NUM_X = 4;
	const int TILE_NUM_Y = 4;
	DirectX::XMFLOAT2 texs_size = { skill_slot_icon->get_tex_width() / TILE_NUM_X,
		skill_slot_icon->get_tex_height() / TILE_NUM_Y };
	skill_slot_icon->begin(graphics.get_dc().Get());
	radius = lerp(radius, 150.0f, 10.0f * elapsed_time);
	for (int i = 0; i < support_skill_slots.size() + 1; i++)
	{
		icon_pos.x = center_pos.x +radius * sinf( DirectX::XMConvertToRadians(add_ang +i *(360/ (TILE_NUM_X * 2))));
		icon_pos.y = center_pos.y +radius * cosf( DirectX::XMConvertToRadians(add_ang +i *(360 / (TILE_NUM_X * 2))));
		skill_slot_icon->render(graphics.get_dc().Get(),
			{ icon_pos.x,icon_pos.y },
			{ 0.2f,0.2f },
			{ 1,1,1,1 },
			0,
			{ i * texs_size.x,0 },
			texs_size);
	}

	for (int i= 0; i < attack_skill_slots.size() + 1; i++)
	{
		
		skill_slot_icon->render(graphics.get_dc().Get(),
			{ i * texs_size.x,center_pos.y },
			{ 0.2f,0.2f },
			{ 1,1,1,lerp(0.0f,1.0f,1.0f * elapsed_time)},
			0,
			{i * texs_size.x,texs_size.y}, texs_size);
	}
	skill_slot_icon->end(graphics.get_dc().Get());


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
void SkillManager::set_support_skill()
{
	support_skill = regeneration_skill.get();
}

//攻撃スキルを使用枠にセット
void SkillManager::set_attack_skill()
{
	attack_skill = magic_bullet_skill.get();
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

			support_skill = support_skill_slots.at(item_current).get();

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

			attack_skill = attack_skill_slots.at(item_current2).get();

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
		type_name = magic_enum::enum_name<SkillSlot::SkillType>(s->skill_type);
		s->debug_gui(type_name);
	}

	for (auto& s : attack_skill_slots)
	{
		std::string type_name;
		type_name = magic_enum::enum_name<SkillSlot::SkillType>(s->skill_type);
		s->debug_gui(type_name);
	}
#endif

}
