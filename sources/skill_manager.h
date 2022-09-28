#pragma once
#include "skill_slot.h"
#include <array>
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

	//サポートスキル詠唱
	void chant_support_skill(Graphics& graphics);
	//攻撃スキル詠唱
	void chant_attack_skill(Graphics& graphics);
	//サポートスキルセット
	void set_support_skill();
	//攻撃スキルセット
	void set_attack_skill();

	//デバッグGUI
	void debug_gui();
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
	std::shared_ptr<SkillSlot> physical_up_skill;
	std::shared_ptr<SkillSlot> regeneration_skill;
	std::shared_ptr<SkillSlot> restraint_skill;
	std::shared_ptr<SkillSlot> magic_bullet_skill;
	std::shared_ptr<SkillSlot> spears_sea_skill;
	//std::shared_ptr<SkillSlot> regeneration_skill;
	std::array<std::shared_ptr<SkillSlot>, 3> support_skill_slots;
	std::array<std::shared_ptr<SkillSlot>, 2> attack_skill_slots;
	//プレイヤーが実際に実行するスキルの箱
	SkillSlot* support_skill;
	SkillSlot* attack_skill;
	bool display_imgui;
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================


};