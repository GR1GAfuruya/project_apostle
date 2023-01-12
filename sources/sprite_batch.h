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
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;	
													 

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
	void render(ID3D11DeviceContext* dc, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale,
	            DirectX::XMFLOAT4 color,
	            float angle);

	void render(ID3D11DeviceContext* dc, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale
		);

	void render(ID3D11DeviceContext* dc, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale,
		DirectX::XMFLOAT4 color,float angle,
		DirectX::XMFLOAT2 tex_pos, DirectX::XMFLOAT2 tex_size);

	void render(ID3D11DeviceContext* dc, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale,
		DirectX::XMFLOAT2 pivot, DirectX::XMFLOAT4 color, float angle,
		DirectX::XMFLOAT2 texpos, DirectX::XMFLOAT2 texsize);

	void begin(ID3D11DeviceContext* dc,
		ID3D11PixelShader* replaced_pixel_shader = nullptr, ID3D11ShaderResourceView* replaced_shader_resource_view = nullptr);

	void end(ID3D11DeviceContext* dc);

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
