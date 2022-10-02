#pragma once
#include "support_skill_slot.h"
#include "attack_skill_slot.h"
#include <array>
#include "sprite_batch.h"
#include "skill_ui.h"
class SkillManager
{
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	SkillManager(Graphics& graphics);
	~SkillManager() {};

	//初期化
	void initialize(Graphics& graphics);
	//更新
	void update(Graphics& graphics, float elapsed_time);
	//描画
	void render(Graphics& graphics);


	void ui_render(Graphics& graphics, float elapsed_time);

	//サポートスキル詠唱
	void chant_support_skill(Graphics& graphics);
	//攻撃スキル詠唱
	void chant_attack_skill(Graphics& graphics);
	//サポートスキルセット
	void set_support_skill(int skill_index);
	//攻撃スキルセット
	void set_attack_skill(int skill_index);

	int select_skill_slot(DirectX::XMFLOAT2 stick_vec);
	//デバッグGUI
	void debug_gui(Graphics& graphics);
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================
	

	//==============================================================
	// 
	// 変数
	// 
	//==============================================================
	//各種スキル
	std::shared_ptr<SupportSkillSlot> physical_up_skill;
	std::shared_ptr<SupportSkillSlot> regeneration_skill;
	std::shared_ptr<SupportSkillSlot> restraint_skill;
	std::shared_ptr<AttackSkillSlot> magic_bullet_skill;
	std::shared_ptr<AttackSkillSlot> spears_sea_skill;
	//std::shared_ptr<SkillSlot> regeneration_skill;
	std::array<std::shared_ptr<SupportSkillSlot>, 3> support_skill_slots;
	std::array<std::shared_ptr<AttackSkillSlot>, 2> attack_skill_slots;
	//プレイヤーが実際に実行するスキルの箱
	SupportSkillSlot* support_skill;
	AttackSkillSlot* attack_skill;

	
	std::unique_ptr<SkillUI> sup_slots_ui;
	std::unique_ptr<SkillUI> atk_slots_ui;
	bool display_imgui;
	bool is_selecting_support_skill;
	bool is_selecting_attack_skill;
	
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================


};