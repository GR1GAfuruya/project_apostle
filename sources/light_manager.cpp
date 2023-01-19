#include "light_manager.h"
#include "user.h"

//==============================================================
// 
//������
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
//�f�X�g���N�^
// 
//==============================================================
LightManager::~LightManager()
{
	lights.clear();
}
//==============================================================
// 
//���C�g�̓o�^
// 
//==============================================================
void LightManager::register_light(std::string name, std::shared_ptr<Light> light)
{
	//ID
	int id = 0;
	//���̖��O
	std::string  unique_name = name;
	for (auto& l : lights)
	{
		//�������O�����Ԃ��Ă�����
		if (name == l.first)
		{
			//���̖��O�̌��ɐ�����t������
			id++;
			name = unique_name + to_string(id);
		}
	}
	//�}�b�v�ɓo�^
	lights[name] = light;
	//���C�g�ɕύX�������O��o�^
	light->name = name;
}

//==============================================================
// 
//���C�g�o�b�t�@�Ƀ��C�g����������
// 
//==============================================================
void LightManager::draw(Graphics& graphics, ID3D11ShaderResourceView** rtv,int rtv_num)
{
	
	//�e�p���C�g�`��
#if CAST_SHADOW
	//shadow_dir_light->light_constants->bind(graphics.get_dc().Get(), 7);
	//light_screen->blit(graphics.get_dc().Get(), rtv, 0, rtv_num, shadow_map_light.Get());
#endif
	//�ʏ탉�C�g�`��
	for (auto& light : lights)
	{
		//�����N���؂�Ă��Ȃ����`�F�b�N
		if (!light.second.expired())
		{
			//�Ď����Ă��郉�C�g�̊֐�����
			light.second.lock()->light_constants->bind(graphics.get_dc().Get(), 7);
			light_screen->blit(graphics.get_dc().Get(), rtv, 0, rtv_num, deferred_light.Get());
		}
		//�v�f���Ȃ��̂�map�̗̈���Ƃ��Ă���Ƃ��Ɍx�����o��
		_ASSERT_EXPR(!light.second.expired(), L"light_map��nullptr�����݂��Ă��܂�\n delete_light()���ĂіY��Ă���\��������܂�");
	}
}
//==============================================================
// 
//�f�o�b�OGUI
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
//���C�g�̍폜
// 
//==============================================================
void LightManager::delete_light(std::string name)
{
	//�w��̃L�[�̗v�f���}�b�v����폜
	lights.erase(name);
}
