#include "light_manager.h"
#include "user.h"

//==============================================================
// 
//初期化
// 
//==============================================================
void LightManager::initialize(Graphics& graphics)
{
	HRESULT hr;
	hr = create_ps_from_cso(graphics.get_device().Get(), "shaders/deferred_light.cso", deferred_light.GetAddressOf());
	hr = create_ps_from_cso(graphics.get_device().Get(), "shaders/deferred_light_shadow.cso", shadow_map_light.GetAddressOf());
	light_screen = std::make_unique<FullscreenQuad>(graphics.get_device().Get());
	lights.clear();
#if CAST_SHADOW
	DirectX::XMFLOAT3 shadow_light_dir = { 1.0f, -1.0f, -1.0 };
	DirectX::XMFLOAT3 shadow_color = { 0.2f, 0.2f, 0.2f };
	shadow_dir_light = std::make_shared<DirectionalLight>(graphics, shadow_light_dir, shadow_color);
	LightManager::instance().register_light("shadow_dir_light", shadow_dir_light);
#endif
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
//==============================================================
// 
//デストラクタ
// 
//==============================================================
LightManager::~LightManager()
{
	lights.clear();
}
//==============================================================
// 
//ライトの登録
// 
//==============================================================
void LightManager::register_light(std::string name, std::shared_ptr<Light> light)
{
	//ID
	int id = 0;
	//元の名前
	std::string  unique_name = name;
	for (auto& l : lights)
	{
		//もし名前がかぶっていたら
		if (name == l.first)
		{
			//元の名前の後ろに数字を付け足す
			id++;
			name = unique_name + to_string(id);
		}
	}
	//マップに登録
	lights[name] = light;
	//ライトに変更した名前を登録
	light->name = name;
}

//==============================================================
// 
//ライトバッファにライトを書き込む
// 
//==============================================================
void LightManager::draw(Graphics& graphics, ID3D11ShaderResourceView** rtv,int rtv_num)
{
	
	//影用ライト描画
#if CAST_SHADOW
	//shadow_dir_light->light_constants->bind(graphics.get_dc().Get(), 7);
	//light_screen->blit(graphics.get_dc().Get(), rtv, 0, rtv_num, shadow_map_light.Get());
#endif
	//通常ライト描画
	for (auto& light : lights)
	{
		//リンクが切れていないかチェック
		if (!light.second.expired())
		{
			//監視しているライトの関数処理
			light.second.lock()->light_constants->bind(graphics.get_dc().Get(), 7);
			light_screen->blit(graphics.get_dc().Get(), rtv, 0, rtv_num, deferred_light.Get());
		}
		//要素がないのにmapの領域をとっているときに警告を出す
		_ASSERT_EXPR(!light.second.expired(), L"light_mapにnullptrが存在しています\n delete_light()を呼び忘れている可能性があります");
	}
}
//==============================================================
// 
//デバッグGUI
// 
//==============================================================
void LightManager::debug_gui()
{
	imgui_menu_bar("Lights", "Light", display_imgui);
	if (display_imgui)
	{
#if CAST_SHADOW
		shadow_dir_light->debug_gui("shadow");
#endif
		for (auto& l : lights)
		{
			if (!l.second.expired())
			{
				l.second.lock()->debug_gui(l.first);
			}
		}
	}
}
//==============================================================
// 
//ライトの削除
// 
//==============================================================
void LightManager::delete_light(std::string name)
{
	//指定のキーの要素をマップから削除
	lights.erase(name);
}
