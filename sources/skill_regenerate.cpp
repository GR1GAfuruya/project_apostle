#include "skill_regenerate.h"
#include "light_manager.h"
//==============================================================
// 
//コンストラクタ
// 
//==============================================================
Regenerate::Regenerate(Graphics& graphics, DirectX::XMFLOAT3* launch_pos, int* health, const int max_health)
{ 
	light_position.reset(launch_pos);
	charactor_health.reset(health);
	recovery_max_health = max_health;

	//ライト生成
	light = make_shared<PointLight>(graphics, position, 15.0f, DirectX::XMFLOAT3(0.4f, 1.8f, 0.5f));
	LightManager::instance().register_light("Regenerate", light);

	initialize(graphics);
}
//==============================================================
// 
//デストラクタ
// 
//==============================================================
Regenerate::~Regenerate()
{
	light_position.release();
	charactor_health.release();
	//ライト消去
	LightManager::instance().delete_light(light->name);
}
//==============================================================
// 
//初期化
// 
//==============================================================
void Regenerate::initialize(Graphics& graphics)
{
	//スキル時間
	life_time = 6.0f;
	//回復周期
	recovery_timer = RECOVERY_SPAN;
	//回復量
	recovery_rate = 5;
	//クールタイム
	cool_time = 9.0f;
}
//==============================================================
// 
//更新
// 
//==============================================================
void Regenerate::update(Graphics& graphics, float elapsed_time)
{
	//体力が最大値以下なら一定時間ごとに回復
	if (*charactor_health < recovery_max_health)
	{
		recovery_timer -= elapsed_time;
		if (recovery_timer <= 0.0f)
		{
			*charactor_health += recovery_rate;
			recovery_timer = RECOVERY_SPAN;
		}
	}
	//ライトの位置更新
	const float offset = 11.0f;
	DirectX::XMFLOAT3 light_pos = { light_position->x ,light_position->y + offset ,light_position->z };
	light->set_position(light_pos);
	//スキル時間の更新
	life_time -= elapsed_time;
	if (life_time < 0)
	{
		skill_end_flag = true;
	}
}
//==============================================================
// 
//描画
// 
//==============================================================
void Regenerate::render(Graphics& graphics)
{
}
//==============================================================
// 
//デバッグGUI
// 
//==============================================================
void Regenerate::debug_gui(string str_id)
{
#if USE_IMGUI
	string name = "Regenerate:" + str_id + "Param";
	ImGui::Begin("Regenerate");
	ImGui::PushID(str_id.c_str());
	/*これより下にパラメーター記述*/
	ImGui::BulletText(name.c_str());

	ImGui::DragFloat("life_time", &life_time);
	ImGui::Checkbox("skill_end_flag", &skill_end_flag);
	
	/*これより上にパラメーター記述*/
	ImGui::PopID();
	ImGui::End();

#endif

}
