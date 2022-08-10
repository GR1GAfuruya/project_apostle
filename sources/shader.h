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

	//描画タイプ
	enum class RenderType
	{
		Forward,
		Deferred
	};

	// 描画開始
	virtual void active(ID3D11DeviceContext* immediate_context, RenderType rt) = 0;
	virtual void render(ID3D11DeviceContext* immediate_context, SkeletalMesh* model, const DirectX::XMFLOAT4X4& world = {}) = 0;
};

using namespace std;

//頂点シェーダー生成
HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader,
	ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_elementDesc, UINT num_elements);
//頂点シェーダー生成
HRESULT create_hs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11HullShader** hull_shader);

//頂点シェーダー生成
HRESULT create_ds_from_cso(ID3D11Device* device, const char* cso_name, ID3D11DomainShader** domain_shader);

//頂点シェーダー生成
HRESULT create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader);

//頂点シェーダー生成
HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);

//頂点シェーダー生成
HRESULT create_cs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader);

void compile_shader(ID3D11Device* device, const wchar_t* shader_name, ID3D11VertexShader* vertex_shader);

void compile_shader(ID3D11Device* device, const wchar_t* shader_name, ID3D11PixelShader* pixel_shader);
