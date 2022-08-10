#include "shader.h"
#include "imgui_include.h"
#include <array>
#include <d3dcompiler.h>
//--------------------------------------------------------------
//  バーテックスシェーダーファイルを作成
//--------------------------------------------------------------
HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader,
	ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_elementDesc, UINT num_elements)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long csoSz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	unique_ptr<unsigned char[]> csoData{ make_unique<unsigned char[]>(csoSz) };
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);

	HRESULT hr{ S_OK };
	hr = device->CreateVertexShader(csoData.get(), csoSz, nullptr, vertex_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (input_layout)
	{
		hr = device->CreateInputLayout(input_elementDesc, num_elements,
			csoData.get(), csoSz, input_layout);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	return hr;
}

//--------------------------------------------------------------
//  ハルシェーダーファイルを作成
//--------------------------------------------------------------
HRESULT create_hs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11HullShader** hull_shader)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long csoSz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	unique_ptr<unsigned char[]> csoData{ make_unique<unsigned char[]>(csoSz) };
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);

	HRESULT hr{ S_OK };
	hr = device->CreateHullShader(csoData.get(), csoSz, nullptr, hull_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	return hr;
}

//--------------------------------------------------------------
//  ドメインシェーダーファイルを作成
//--------------------------------------------------------------
HRESULT create_ds_from_cso(ID3D11Device* device, const char* cso_name, ID3D11DomainShader** domain_shader)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long csoSz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	unique_ptr<unsigned char[]> csoData{ make_unique<unsigned char[]>(csoSz) };
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);

	HRESULT hr{ S_OK };
	hr = device->CreateDomainShader(csoData.get(), csoSz, nullptr, domain_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	return hr;
}

//--------------------------------------------------------------
//  ジオメトリシェーダーファイルを作成
//--------------------------------------------------------------
HRESULT create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long csoSz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	unique_ptr<unsigned char[]> csoData{ make_unique<unsigned char[]>(csoSz) };
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);

	HRESULT hr{ S_OK };
	hr = device->CreateGeometryShader(csoData.get(), csoSz, nullptr, geometry_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	return hr;
}

//--------------------------------------------------------------
//  ピクセルシェーダーファイルを作成
//--------------------------------------------------------------
HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long csoSz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	unique_ptr<unsigned char[]>csoData{ make_unique<unsigned char[]>(csoSz) };
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);

	HRESULT hr{ S_OK };
	hr = device->CreatePixelShader(csoData.get(), csoSz, nullptr, pixel_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return hr;
}

//--------------------------------------------------------------
//  コンピュートシェーダーファイルを作成
//--------------------------------------------------------------
HRESULT create_cs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long csoSz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	unique_ptr<unsigned char[]>csoData{ make_unique<unsigned char[]>(csoSz) };
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);

	HRESULT hr{ S_OK };
	hr = device->CreateComputeShader(csoData.get(), csoSz, nullptr, compute_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return hr;
}

void compile_shader(ID3D11Device* device, const wchar_t* shader_name, ID3D11VertexShader* vertex_shader)
{
#ifdef USE_IMGUI
	ImGui::Begin("compile_shader");
	if (ImGui::Button("compile"))
	{
		std::array<D3D_SHADER_MACRO, 2> macros = { {
		{"DEFINE_MACRO", "float4(0, 1, 1, 1)"},
		{nullptr, nullptr},
	  } };
		UINT compileFlag = 0;
		//    compileFlag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		compileFlag |= D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_STRICTNESS;
		const char* entryPoint = "main";
		const char* shaderTarget = "vs_5_0";
		Microsoft::WRL::ComPtr<ID3DBlob> pShaderBlob, pErrorMsg = nullptr;
		//シェーダのコンパイル
		D3DCompileFromFile(
			shader_name,
			macros.data(),
			nullptr,
			entryPoint,
			shaderTarget,
			compileFlag,
			0,
			pShaderBlob.GetAddressOf(),
			pErrorMsg.GetAddressOf());
		//ID3D11ComputeShaderの作成
		device->CreateVertexShader(
			pShaderBlob->GetBufferPointer(),
			pShaderBlob->GetBufferSize(),
			nullptr,
			&vertex_shader);
	}
	ImGui::End();
#endif
}

void compile_shader(ID3D11Device* device, const wchar_t* shader_name, ID3D11PixelShader* pixel_shader)
{
#ifdef USE_IMGUI
	ImGui::Begin("compile_shader");
	if (ImGui::Button("compile"))
	{
		std::array<D3D_SHADER_MACRO, 2> macros = { {
		{"DEFINE_MACRO", "float4(0, 1, 1, 1)"},
		{nullptr, nullptr},
	  } };
		UINT compileFlag = 0;
		//    compileFlag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		compileFlag |= D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_STRICTNESS;
		const char* entryPoint = "main";
		const char* shaderTarget = "ps_5_0";
		Microsoft::WRL::ComPtr<ID3DBlob> pShaderBlob, pErrorMsg = nullptr;
		//シェーダのコンパイル
		D3DCompileFromFile(
			shader_name,
			macros.data(),
			nullptr,
			entryPoint,
			shaderTarget,
			compileFlag,
			0,
			pShaderBlob.GetAddressOf(),
			pErrorMsg.GetAddressOf());
		//ID3D11ComputeShaderの作成
		device->CreatePixelShader(
			pShaderBlob->GetBufferPointer(),
			pShaderBlob->GetBufferSize(),
			nullptr,
			&pixel_shader);
	}
	ImGui::End();
#endif
}


