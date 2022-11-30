#include "material.h"
#include "texture.h"

//==============================================================
// 
// �R���X�g���N�^
// 
//==============================================================
Material::Material(Graphics& graphics)
{
}

//==============================================================
// 
//SRV��GPU�ɓ]��
// 
//==============================================================
void Material::transfer_shader_resource_view(Graphics& graphics)
{
	graphics.get_dc().Get()->PSSetShaderResources(
		MATERIAL_START_SLOT, shader_resources.size(), shader_resources.data()->GetAddressOf());
}

//==============================================================
// 
//SRV�����X�g�ɓo�^
// 
//==============================================================
void Material::register_shader_resource(ID3D11Device* device, const wchar_t* filename)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	load_texture_from_file(device, filename, shader_resource_view.ReleaseAndGetAddressOf(), &texture2d_desc);

	shader_resources.push_back(shader_resource_view);
}

//==============================================================
// 
//�s�N�Z���V�F�[�_�[����
// 
//==============================================================
void Material::create_pixel_shader(ID3D11Device* device, const char* cso_name)
{
	create_ps_from_cso(device, cso_name, pixel_shader.ReleaseAndGetAddressOf());
}
