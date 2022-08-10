#include "SpriteDissolve.h"
#include "misc.h"

#include <sstream>

// UNIT.04
#include <functional>

// UNIT.05
//#include <WICTextureLoader.h>

// UNIT.10
#include "texture.h"
#include "shader.h"

SpriteDissolve::SpriteDissolve(ID3D11Device* device, const wchar_t* filename, size_t maxSprites, const wchar_t* mask_texture_filename)
	:  maxVertices(maxSprites * 6)
{
	HRESULT hr{ S_OK };

	//頂点情報のセット
	//vertex vertices[]
	//{
	//	{ { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {0, 0} },//頂点左上
	//	{ { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {1, 0} },//頂点右上
	//	{ { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {0, 1} },//頂点左下
	//	{ { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {1, 1} },//頂点右下
	//};
	std::unique_ptr<vertex[]> vertices{ std::make_unique<vertex[]>(maxVertices) };

	//頂点バッファオブジェクトの生成
	D3D11_BUFFER_DESC bufferDesc{}; //	https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee416048(v=vs.85)
	bufferDesc.ByteWidth = sizeof(vertex) * maxVertices; //バッファーのサイズ
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //バッファーで想定されている読み込みおよび書き込みの方法を識別。　更新頻度は重要な要素
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //バッファーをどのようにパイプラインにバインドするかを識別		フラグは論理和で組み合わせられる
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPUアクセスのフラグ	必要ない場合は０
	bufferDesc.MiscFlags = 0;
	//その他のフラグ（https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee416267(v=vs.85)　を参照）
	bufferDesc.StructureByteStride = 0; //構造体が構造化バッファーを表す場合、その構造体のサイズ (バイト単位) です。
	D3D11_SUBRESOURCE_DATA subresourceData{};
	//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee416284(v=vs.85)
	subresourceData.pSysMem = vertices.get(); //初期化データへのポインター
	subresourceData.SysMemPitch = 0; //テクスチャーにある1本の線の先端から隣の線までの距離
	subresourceData.SysMemSlicePitch = 0;; //1つの深度レベルの先端から隣の深度レベルまでの距離

	hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertex_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//定数バッファ
	scroll_constants = std::make_unique<Constants<SCROLL_CONSTANTS>>(device);

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	// UNIT.10
	create_vs_from_cso(device, "shaders/sprite_dissolve_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(),
		input_element_desc, _countof(input_element_desc));
	create_ps_from_cso(device, "shaders/sprite_dissolve_ps.cso", pixel_shader.GetAddressOf());


	hr = load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = load_texture_from_file(device, mask_texture_filename, mask_shader_resource_view.GetAddressOf(), &mask_texture2d_desc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


inline auto rotate(float& x, float& y, float cx, float cy, float cos, float sin)
{
	x -= cx;
	y -= cy;


	float tx{ x }, ty{ y };
	x = cos * tx + -sin * ty;
	y = sin * tx + cos * ty;

	x += cx;
	y += cy;
};


void SpriteDissolve::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy,		//矩形の左上の座標(スクリーン座標系)
	float dw, float dh,		//矩形のサイズ(スクリーン座標系)
	float r, float g, float b, float a,
	float angle/*degree*/
)
{
	// UNIT.06
	render(immediate_context, dx, dy, dw, dh, r, g, b, a, angle, 0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void SpriteDissolve::render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh)
{
	render(immediate_context, dx, dy, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void SpriteDissolve::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy,		//矩形の左上の座標(スクリーン座標系)
	float dw, float dh,		//矩形のサイズ(スクリーン座標系)
	float r, float g, float b, float a,
	float angle/*degree*/,
	float sx, float sy, float sw, float sh
)
{
	//スクリーン(ビューポート)のサイズを取得する
	D3D11_VIEWPORT viewport{}; //ビューポートの寸法の定義　https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ns-d3d11-d3d11_viewport
	UINT numViewports{ 1 };
	immediate_context->RSGetViewports(&numViewports, &viewport); //ラスタライザステージにバインドされたビューポートの配列を取得

	//renderメンバ関数の引数から矩形の各頂点の位置を計算する
	// (x0, y0) *----* (x1, y1)
	//			|   /|
	//			|  / |
	//			| /	 |
	//			|/	 |
	// (x2, y2),*----* (x3, y3)


	// left-top
	float x0{ dx };
	float y0{ dy };
	//right-top
	float x1{ dx + dw };
	float y1{ dy };
	//left-bottom
	float x2{ dx };
	float y2{ dy + dh };
	//right-bottom
	float x3{ dx + dw };
	float y3{ dy + dh };





	//回転の中心を矩形の中心点にした場合
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;
	float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
	float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
	rotate(x0, y0, cx, cy, cos, sin);
	rotate(x1, y1, cx, cy, cos, sin);
	rotate(x2, y2, cx, cy, cos, sin);
	rotate(x3, y3, cx, cy, cos, sin);


	////スクリーン座標系からNDCへの座標返還を行う
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	//テクセル座標系からテクスチャ座標系への変換
	float u0{ sx / texture2d_desc.Width };
	float v0{ sy / texture2d_desc.Height };

	float u1{ (sx + sw) / texture2d_desc.Width };
	float v1{ (sy + sh) / texture2d_desc.Height };

	vertices.push_back({ {x0, y0, 0}, {r, g, b, a}, {u0, v0} });
	vertices.push_back({ {x1, y1, 0}, {r, g, b, a}, {u1, v0} });
	vertices.push_back({ {x2, y2, 0}, {r, g, b, a}, {u0, v1} });
	vertices.push_back({ {x2, y2, 0}, {r, g, b, a}, {u0, v1} });
	vertices.push_back({ {x1, y1, 0}, {r, g, b, a}, {u1, v0} });
	vertices.push_back({ {x3, y3, 0}, {r, g, b, a}, {u1, v1} });


}

void SpriteDissolve::begin(ID3D11DeviceContext* immediate_context,
	ID3D11PixelShader* replaced_pixel_shader/*UNIT.10*/, ID3D11ShaderResourceView* replaced_shader_resource_view/*UNIT.10*/)
{
	vertices.clear();
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0) : immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	// UNIT.10
	//immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
	if (replaced_shader_resource_view)
	{
		HRESULT hr{ S_OK };
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		replaced_shader_resource_view->GetResource(resource.GetAddressOf());

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		texture2d->GetDesc(&texture2d_desc);

		immediate_context->PSSetShaderResources(0, 1, &replaced_shader_resource_view);
		
	}
	else
	{
		immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	
		immediate_context->PSSetShaderResources(1, 1, mask_shader_resource_view.GetAddressOf());
	}

	//定数バッファの更新
	scroll_constants->bind(immediate_context, 2, ::CB_FLAG::PS_VS);
}

void SpriteDissolve::end(ID3D11DeviceContext* immediate_context)
{
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	size_t vertex_count = vertices.size();
	_ASSERT_EXPR(maxVertices >= vertex_count, "Buffer overflow");
	vertex* data{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	if (data != nullptr)
	{
		const vertex* p = vertices.data();
		memcpy_s(data, maxVertices * sizeof(vertex), p, vertex_count * sizeof(vertex));
	}

	immediate_context->Unmap(vertex_buffer.Get(), 0);


	//頂点バッファーのバインド
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	//プリミティブタイプおよびデータの順序に関する情報のバインド
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//TRIANGLELISTにしたら3角形１つになった

	//入力レイアウトオブジェクトのバインド
	immediate_context->IASetInputLayout(input_layout.Get());
	//シェーダーのバインド

	//プリミティブの描画
	immediate_context->Draw(static_cast<UINT>(vertex_count), 0);

	// UNIT.10
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	shader_resource_view.Get()->GetResource(resource.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	texture2d->GetDesc(&texture2d_desc);
}
