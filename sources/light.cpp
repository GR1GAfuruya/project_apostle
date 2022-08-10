#include "light.h"
#include "imgui_include.h"
Light::Light(Graphics& graphics)
{
	light_constants = std::make_unique<Constants<CBDirLightParam>>(graphics.get_device().Get());
}


DirectionalLight::DirectionalLight(Graphics& graphics,DirectX::XMFLOAT3 dir, float r, float g, float b) :Light(graphics)
{
	//���s���́o����x�A����y�A����z�A0.0�p
	light_constants->data.dir_light_dir = { dir.x, dir.y, dir.z, 0.0f };
	light_constants->data.dir_light_color = { r,g,b,1.0f };
}

void DirectionalLight::debug_gui(int unique_id)
{
	string id = std::to_string(unique_id);
	string name = "dir_light:id " + id;
#if USE_IMGUI
	ImGui::Begin("light");
	if (ImGui::CollapsingHeader(name.c_str()))
	{
		ImGui::DragFloat3(string("dir " + id).c_str(), &light_constants->data.dir_light_dir.x, 0.1f );
		ImGui::DragFloat3(string("color " + id).c_str(), &light_constants->data.dir_light_color.x, 0.1f);
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


PointLight::PointLight(Graphics& graphics, DirectX::XMFLOAT3 position, float distance, float r, float g, float b) :Light(graphics)
{
	//�_�����́o�ʒux�A�ʒuy�A�ʒuz�A���������p
	light_constants->data.dir_light_dir = { position.x, position.y, position.z, distance };
	light_constants->data.dir_light_color = { r,g,b,1.0f };
	
}

void PointLight::debug_gui(int unique_id)
{
	string id = std::to_string(unique_id);
	string name = "point_light:id " + id;
#if USE_IMGUI
	ImGui::Begin("light");
	if (ImGui::CollapsingHeader(name.c_str()))
	{
		ImGui::DragFloat3(string("position " + id).c_str(), &light_constants->data.dir_light_dir.x, 0.5f);
		ImGui::DragFloat3(string("color " + id).c_str(), &light_constants->data.dir_light_color.x, 0.1f, 0, 255);
		ImGui::DragFloat(string("dir " + id).c_str(), &light_constants->data.dir_light_dir.w,0.1f,0,30);
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


