#include "light.h"
#include "imgui_include.h"
//==============================================================
// 
//基底クラスのコンストラクタ
// 
//==============================================================
Light::Light()
{
	Graphics& graphics = Graphics::instance();
	light_constants = std::make_unique<Constants<DirLightParam>>(graphics.get_device().Get());
}

//==============================================================
// 
//平行光コンストラクタ
// 
//==============================================================
DirectionalLight::DirectionalLight(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 color) :Light()
{
	//平行光は｛方向x、方向y、方向z、0.0｝
	light_constants->data.dir_light_dir = { dir.x, dir.y, dir.z, 0.0f };
	light_constants->data.dir_light_color = { color.x, color.y, color.z, 1.0f };
}
//==============================================================
// 
//デバッグGUI
// 
//==============================================================
void DirectionalLight::debug_gui(std::string name)
{
#if USE_IMGUI
	ImGui::Begin("light");
	if (ImGui::CollapsingHeader(name.c_str()))
	{
		ImGui::DragFloat3(string("dir " + name).c_str(), &light_constants->data.dir_light_dir.x, 0.1f);
		ImGui::DragFloat3(string("color " + name).c_str(), &light_constants->data.dir_light_color.x, 0.1f, 0.0f);
	}
	ImGui::End();
#endif // 0
}
//==============================================================
// 
//方向設定
// 
//==============================================================
void DirectionalLight::set_direction(DirectX::XMFLOAT3 dir)
{
	light_constants->data.dir_light_dir = { dir.x, dir.y, dir.z, 0.0f };
}
//==============================================================
// 
//色設定
// 
//==============================================================
void DirectionalLight::set_color(DirectX::XMFLOAT3 color)
{
	light_constants->data.dir_light_color = { color.x, color.y, color.z, 1.0f };
}
//==============================================================
// 
//方向取得
// 
//==============================================================
DirectX::XMFLOAT3 DirectionalLight::get_direction()
{
	return DirectX::XMFLOAT3(light_constants->data.dir_light_dir.x, light_constants->data.dir_light_dir.y, light_constants->data.dir_light_dir.z);
}
//==============================================================
// 
//色取得
// 
//==============================================================
DirectX::XMFLOAT3 DirectionalLight::get_color()
{
	return DirectX::XMFLOAT3(light_constants->data.dir_light_color.x, light_constants->data.dir_light_color.y, light_constants->data.dir_light_color.z);
}



//==============================================================
// 
//ポイントライトコンストラクタ
// 
//==============================================================
PointLight::PointLight(DirectX::XMFLOAT3 position, float distance, DirectX::XMFLOAT3 color) :Light()
{
	//点光源は｛位置x、位置y、位置z、減衰距離｝
	light_constants->data.dir_light_dir = { position.x, position.y, position.z, distance };
	light_constants->data.dir_light_color = { color.x, color.y, color.z, 1.0f };

}
//==============================================================
// 
//デバッグGUI
// 
//==============================================================
void PointLight::debug_gui(std::string name)
{
#if USE_IMGUI
	ImGui::Begin("light");
	if (ImGui::CollapsingHeader(name.c_str()))
	{
		ImGui::DragFloat3(string("position " + name).c_str(), &light_constants->data.dir_light_dir.x, 0.5f);
		ImGui::DragFloat3(string("color " + name).c_str(), &light_constants->data.dir_light_color.x, 0.1f, 0);
		ImGui::DragFloat(string("distance " + name).c_str(), &light_constants->data.dir_light_dir.w, 0.1f, 1, 500);
	}
	ImGui::End();
#endif // 0
}

//==============================================================
// 
//位置設定
// 
//==============================================================
void PointLight::set_position(DirectX::XMFLOAT3 position)
{
	light_constants->data.dir_light_dir.x = position.x;
	light_constants->data.dir_light_dir.y = position.y;
	light_constants->data.dir_light_dir.z = position.z;
}

//==============================================================
// 
//色設定
// 
//==============================================================
void PointLight::set_color(DirectX::XMFLOAT3 color)
{
	light_constants->data.dir_light_color = { color.x, color.y, color.z, 1.0f };
}

//==============================================================
// 
//範囲設定
// 
//==============================================================
void PointLight::set_distance(float d)
{
	light_constants->data.dir_light_dir.w = d;
}

//==============================================================
// 
//位置取得
// 
//==============================================================
DirectX::XMFLOAT3 PointLight::get_position()
{
	return DirectX::XMFLOAT3(light_constants->data.dir_light_dir.x, light_constants->data.dir_light_dir.y, light_constants->data.dir_light_dir.z);
}
//==============================================================
// 
//方向取得
// 
//==============================================================
DirectX::XMFLOAT3 PointLight::get_color()
{
	return DirectX::XMFLOAT3(light_constants->data.dir_light_color.x, light_constants->data.dir_light_color.y, light_constants->data.dir_light_color.z);
}

float PointLight::get_distance()
{
	return light_constants->data.dir_light_dir.w;
}


