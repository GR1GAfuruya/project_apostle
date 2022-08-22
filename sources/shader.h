#pragma once
#include <WICTextureLoader.h>


#include <wrl.h>
using namespace Microsoft::WRL;

#include <string>
#include<map>
#include "misc.h"
#include <memory>
#include "skeletal_mesh.h"

class Shader
{
public:
	Shader() {}
	Shader(ID3D11Device* device) {}
	virtual ~Shader() {}

	// �`��J�n
	virtual void active(ID3D11DeviceContext* immediate_context);

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> f_pixel_shader;//�f�t�H���g�̓t�H���[�h�����_�����O
};


using namespace std;

//���_�V�F�[�_�[����
HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader,
	ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_elementDesc, UINT num_elements);
//���_�V�F�[�_�[����
HRESULT create_hs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11HullShader** hull_shader);

//���_�V�F�[�_�[����
HRESULT create_ds_from_cso(ID3D11Device* device, const char* cso_name, ID3D11DomainShader** domain_shader);

//���_�V�F�[�_�[����
HRESULT create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader);

//���_�V�F�[�_�[����
HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);

//���_�V�F�[�_�[����
HRESULT create_cs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader);

void compile_shader(ID3D11Device* device, const wchar_t* shader_name, ID3D11VertexShader* vertex_shader);

void compile_shader(ID3D11Device* device, const wchar_t* shader_name, ID3D11PixelShader* pixel_shader);
