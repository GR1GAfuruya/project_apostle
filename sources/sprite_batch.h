#pragma once


#include <d3d11.h>
// UNIT.10
#include <wrl.h>
#include <directxmath.h>

// UNIT.09
#include <memory>
#include <vector>

#include "constant.h"

class SpriteBatch
{
public:
	//メンバ変数
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee420044(v=vs.85) 実行プログラム
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee419858(v=vs.85)	実行プログラム
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee419857(v=vs.85)	入力アセンブラー ステージの入力データにアクセス
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;		//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee419532(v=vs.85)　構造化されていないメモリーであるバッファー リソースにアクセス
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;		//https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/nn-d3d11-id3d11shaderresourceview 
													   //シェーダー-リソース-ビューインターフェイスは、シェーダーがレンダリング中にアクセスできるサブリソースを指定

	D3D11_TEXTURE2D_DESC texture2d_desc;

	//定数バッファ
	struct SCROLL_CONSTANTS
	{
		DirectX::XMFLOAT2 scroll_direction;
		DirectX::XMFLOAT2 scroll_speed;
	};
	std::unique_ptr<Constants<SCROLL_CONSTANTS>> scroll_constants;

	DirectX::XMFLOAT2 scroll_direction;

	D3D11_TEXTURE2D_DESC get_texture2d_desc()const { return texture2d_desc; }
	float get_tex_width()const { return static_cast<float>(texture2d_desc.Width); }
	float get_tex_height()const { return static_cast<float>(texture2d_desc.Height); }
	//メンバ関数
	void render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale,
	            DirectX::XMFLOAT4 color,
	            float angle);

	void render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale
		);

	void render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale,
		DirectX::XMFLOAT4 color,float angle,
		DirectX::XMFLOAT2 tex_pos, DirectX::XMFLOAT2 tex_size);

	void begin(ID3D11DeviceContext* immediate_context,
		ID3D11PixelShader* replaced_pixel_shader = nullptr/*UNIT.10*/, ID3D11ShaderResourceView* replaced_shader_resource_view = nullptr/*UNIT.10*/);

	void end(ID3D11DeviceContext* immediate_context);

	SpriteBatch(ID3D11Device* device, const wchar_t* filename, size_t maxSprites);
	~SpriteBatch();

	//頂点フォーマット
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
private:
	//メンバ変数
	const size_t maxVertices;
	std::vector<vertex> vertices;
};
