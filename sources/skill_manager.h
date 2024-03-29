#pragma once
#include <array>
#include "sprite_batch.h"
#include "skill_ui.h"
#include "physical_up_launcher.h"
#include "regenerate_launcher.h"
#include "restraint_launcher.h"
#include "skill_spears_sea.h"
#include "magic_bullet_launcher.h"
#include "spears_sea_launcher.h"
#include "slash_wave_launcher.h"
#include "lightning_rain_launcher.h"
#define SP_SKILLTYPE SkillManager::SupportSkillType
#define ATK_SKILLTYPE SkillManager::AttackSkillType
class SkillManager
{
public:
	//==============================================================
	// 
	// 構造体、列挙型
	// 
	//==============================================================
	enum class SupportSkillType
	{
		PHYSICAL_UP,
		REGENERATE,
		RESTRAINNT,
		SUP_SKILL_MAX,
		TEST,
	};
	enum class AttackSkillType
	{
		MAGICBULLET,
		SPEARS_SEA,
		SLASH_WAVE,
		LIGHTNING_RAIN,
		ATK_SKILL_MAX,
	};

	//==============================================================
	// 
	// public関数
	// 
	//==============================================================
	SkillManager();
	~SkillManager() {};

	//初期化
	void initialize();
	//更新
	void update(float elapsed_time);
	//描画
	void render(Camera* camera);

	//UI描画
	void ui_render(float elapsed_time);

	SupportSkillType get_selected_sup_skill_type() { return selected_sup_skill_type; }

	AttackSkillType get_selected_atk_skill_type() { return selected_atk_skill_type; }

	//スキル詠唱
	bool chant_physical_up(DirectX::XMFLOAT3* launch_pos, float* add_run_speed, float* add_jump_speed);

	bool chant_regenerate(DirectX::XMFLOAT3* launch_pos, int* health, const int max_health);

	bool chant_restraint(DirectX::XMFLOAT3* target_pos, float* down_speed);

	//攻撃スキル詠唱
	bool chant_magic_bullet(DirectX::XMFLOAT3* launch_pos, DirectX::XMFLOAT3* dir);

	bool chant_spear_sea(DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos);

	bool chant_slash_wave(DirectX::XMFLOAT3* launch_pos, DirectX::XMFLOAT3* dir);

	bool chant_lightning_rain(DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos);

	//スキルクールタイム短縮
	void cool_time_reduction();

	//----スキル当たり判定----//
	//MagickBullet
	void judge_magic_bullet_vs_enemy(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera);
	//SpearSea
	void judge_spear_sea_vs_enemy(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera);
	void judge_slash_wave_vs_enemy(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera);
	void judge_lightning_rain_vs_enemy(Capsule object_colider, AddDamageFunc damaged_func, Camera* camera);
	//スキルセレクト中かどうか
	bool is_selecting_skill() {
		if (is_selecting_support_skill) return true;
		if (is_selecting_attack_skill) return true;
		return false;
	}

	int select_skill_slot(DirectX::XMFLOAT2 stick_vec, int slot_num);
	//デバッグGUI
	void debug_gui();

private:
	//サポートスキルセット
	void set_support_skill(int skill_index);
	//攻撃スキルセット
	void set_attack_skill(int skill_index);

	//==============================================================
	// 
	// 変数
	// 
	//==============================================================
	//プレイヤーが実際に実行するスキルの箱
	std::shared_ptr<PhysicalUpLauncher> physical_up;
	std::shared_ptr<RegenerateLauncher> regenerate;
	std::shared_ptr<RestraintLauncher> restraint;

	std::shared_ptr<MagicBulletLauncher> magick_bullet;
	std::shared_ptr<SpearSeaLauncher> spear_sea;
	std::shared_ptr<SlashWaveLauncher> slash_wave;
	std::shared_ptr<LightningRainLauncher> lightning_rain;

	std::array< std::shared_ptr<SkillLauncher>, static_cast<int>(SupportSkillType::SUP_SKILL_MAX)> sup_skill_list;
	std::array< std::shared_ptr<SkillLauncher>, static_cast<int>(AttackSkillType::ATK_SKILL_MAX)> atk_skill_list;

	//装備中のスキル
	SkillLauncher* selected_sup_skill;
	SkillLauncher* selected_atk_skill;
	//装備中のスキルの種類
	SupportSkillType selected_sup_skill_type;
	AttackSkillType selected_atk_skill_type;
	//スキルのUI
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