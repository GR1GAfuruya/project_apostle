#include "light.h"
#include "imgui_include.h"
Light::Light(Graphics& graphics)
{
	light_constants = std::make_unique<Constants<DirLightParam>>(graphics.get_device().Get());
}


DirectionalLight::DirectionalLight(Graphics& graphics,DirectX::XMFLOAT3 dir, float r, float g, float b) :Light(graphics)
{
	//平行光は｛方向x、方向y、方向z、0.0｝
	light_constants->data.dir_light_dir = { dir.x, dir.y, dir.z, 0.0f };
	light_constants->data.dir_light_color = { r,g,b,1.0f };
}

void DirectionalLight::debug_gui(std::string name)
{
#if USE_IMGUI
	ImGui::Begin("light");
	if (ImGui::CollapsingHeader(name.c_str()))
	{
		ImGui::DragFloat3(string("dir " + name).c_str(), &light_constants->data.dir_light_dir.x, 0.1f );
		ImGui::DragFloat3(string("color " + name).c_str(), &light_constants->data.dir_light_color.x, 0.1f,0.0f);
	}
	ImGui::End();
#endif // 0
}

void DirectionalLight::set_direction(DirectX::XMFLOAT3 dir)
{
	light_constants->data.dir_light_dir = { dir.x, dir.y, dir.z, 0.0f };
}
void DirectionalLight::set_color(float r, float g, float b)
{
	light_constants->data.dir_light_color = { r,g,b,1.0f };
}

DirectX::XMFLOAT3 DirectionalLight::get_direction()
{
	return DirectX::XMFLOAT3(light_constants->data.dir_light_dir.x, light_constants->data.dir_light_dir.y, light_constants->data.dir_light_dir.z);
}

DirectX::XMFLOAT3 DirectionalLight::get_color()
{
	return DirectX::XMFLOAT3(light_constants->data.dir_light_color.x, light_constants->data.dir_light_color.y, light_constants->data.dir_light_color.z);
}


PointLight::PointLight(Graphics& graphics, DirectX::XMFLOAT3 position, float distance, float r, float g, float b) :Light(graphics)
{
	//点光源は｛位置x、位置y、位置z、減衰距離｝
	light_constants->data.dir_light_dir = { position.x, position.y, position.z, distance };
	light_constants->data.dir_light_color = { r,g,b,1.0f };
	
}

void PointLight::debug_gui(std::string name)
{
#if USE_IMGUI
	ImGui::Begin("light");
	if (ImGui::CollapsingHeader(name.c_str()))
	{
		ImGui::DragFloat3(string("position " + name).c_str(), &light_constants->data.dir_light_dir.x, 0.5f);
		ImGui::DragFloat3(string("color " + name).c_str(), &light_constants->data.dir_light_color.x, 0.1f, 0);
		ImGui::DragFloat(string("distance " + name).c_str(), &light_constants->data.dir_light_dir.w,0.1f,1,500);
	}
	ImGui::End();
#endif // 0
}

void PointLight::set_position(DirectX::XMFLOAT3 position)
{
	light_constants->data.dir_light_dir.x = position.x;
	light_constants->data.dir_light_dir.y = position.y;
	light_constants->data.dir_light_dir.z = position.z;
}
void PointLight::set_color(float r, float g, float b)
{
	light_constants->data.dir_light_color = { r,g,b,1.0f };
}
void PointLight::set_distance(float d)
{
	light_constants->data.dir_light_dir.w = d;
}

DirectX::XMFLOAT3 PointLight::get_position()
{
	return DirectX::XMFLOAT3(light_constants->data.dir_light_dir.x, light_constants->data.dir_light_dir.y, light_constants->data.dir_light_dir.z);
}

DirectX::XMFLOAT3 PointLight::get_color()
{
	return DirectX::XMFLOAT3(light_constants->data.dir_light_color.x, light_constants->data.dir_light_color.y, light_constants->data.dir_light_color.z);
}

float PointLight::get_distance()
{
	return light_constants->data.dir_light_dir.w;
}


