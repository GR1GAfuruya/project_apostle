#pragma once
#include<d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <wrl.h>
#include<string>

#include "constant.h"

class Sprite
{
public:
	//メンバ変数
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;	//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee420044(v=vs.85) 実行プログラム
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;	//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee419858(v=vs.85)	実行プログラム
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;	//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee419857(v=vs.85)	入力アセンブラー ステージの入力データにアクセス
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;		//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee419532(v=vs.85)　構造化されていないメモリーであるバッファー リソースにアクセス
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;		//https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/nn-d3d11-id3d11shaderresourceview 
													   //シェーダー-リソース-ビューインターフェイスは、シェーダーがレンダリング中にアクセスできるサブリソースを指定
	//定数バッファ
	struct SCROLL_CONSTANTS
	{
		DirectX::XMFLOAT2 scroll_direction;
		DirectX::XMFLOAT2 scroll_speed;
	}scroll_data;
	std::unique_ptr<Constants<SCROLL_CONSTANTS>> scroll_constants;
	DirectX::XMFLOAT2 scroll_direction;

	D3D11_TEXTURE2D_DESC texture2dDesc;
	//メンバ関数
	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh,
		float r, float g, float b, float a,
		float angle, float sx, float sy, float sw, float sh);

	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh);

	void textout(ID3D11DeviceContext* immediate_context, std::string s,
		float x, float y, float w, float h, float r, float g, float b, float a);

	Sprite(ID3D11Device* device, const wchar_t* filename);
	Sprite(ID3D11Device* device);
	~Sprite();

	//頂点フォーマット
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
private:
	
};
