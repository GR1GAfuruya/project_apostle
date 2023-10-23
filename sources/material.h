#pragma once
#include "graphics.h"

class Material
{
public:
	Material();
	~Material() {}

	//ID�擾
	string get_id() { return id; }
	//SRV��GPU�ɓ]��
	void transfer_shader_resource_view();
	//SRV�����X�g�ɓo�^
	void register_shader_resource(ID3D11Device* device, const wchar_t* filename);

	void create_pixel_shader(ID3D11Device* device, const char* cso_name);

	void reload();

	const Microsoft::WRL::ComPtr<ID3D11VertexShader> get_vs() { return vertex_shader.Get(); }
	const Microsoft::WRL::ComPtr<ID3D11PixelShader> get_ps() { return pixel_shader.Get(); }

protected:
	//���_�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	//�s�N�Z���V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	//�V�F�[�_�[���\�[�X�r���[�̃R���e�i
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> shader_resources;
	//HLSL��ł̃��W�X�^�[�̃X�^�[�g�ʒu
	const int MATERIAL_START_SLOT = 20;
	//�}�e���A��ID
	string id;
};