#pragma once
#include "skill_slot.h"
class SkillManager
{
public:
	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	SkillManager();
	~SkillManager();

	void initialize(Graphics& graphics);
	void update(Graphics& graphics, float elapsed_time);
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
	std::shared_ptr<SkillSlot> regeneration_skill;

	//プレイヤーが実際に実行するスキルの箱
	SkillSlot* support_skill;
	SkillSlot* attack_skill;
	//==============================================================
	// 
	// 定数
	// 
	//==============================================================


};