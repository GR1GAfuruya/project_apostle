#include "sprite_batch.h" // UNIT.09
#include "misc.h"

#include <sstream>

#include <functional>


#include "texture.h"
#include "shader.h"


#if _DEBUG
CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
#else
CONST LONG SCREEN_WIDTH{ 1920 };
CONST LONG SCREEN_HEIGHT{ 1080 };
#endif
//解像度による差分の倍率
CONST FLOAT MAGNI_RESOLUTION_WIDTH{ SCREEN_WIDTH / 1280.0f };
//解像度による差分の倍率
CONST FLOAT MAGNI_RESOLUTION_HEIGHT{ SCREEN_HEIGHT / 720.0f };

SpriteBatch::SpriteBatch(ID3D11Device* device, const wchar_t* filename, size_t maxSprites)
	:  maxVertices(maxSprites * 6)
{
	HRESULT hr{S_OK};

	//頂点情報のセット
	//vertex vertices[]
	//{
	//	{ { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {0, 0} },//頂点左上
	//	{ { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {1, 0} },//頂点右上
	//	{ { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {0, 1} },//頂点左下
	//	{ { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {1, 1} },//頂点右下
	//};
	const std::unique_ptr<vertex[]> vertices = std::make_unique<vertex[]>(maxVertices);

	//頂点バッファオブジェクトの生成
	D3D11_BUFFER_DESC bufferDesc{}; //	https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee416048(v=vs.85)
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(vertex) * maxVertices);//バッファーのサイズ
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
	create_vs_from_cso(device, "shaders/sprite_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(),
	                   input_element_desc, _countof(input_element_desc));
	create_ps_from_cso(device, "shaders/sprite_ps.cso", pixel_shader.GetAddressOf());


	hr = load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
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


void SpriteBatch::render(ID3D11DeviceContext* dc,
                         DirectX::XMFLOAT2 position,
                         //矩形の左上の座標(スクリーン座標系)
                         DirectX::XMFLOAT2 scale,
                         //矩形のサイズ(スクリーン座標系)
                         DirectX::XMFLOAT4 color,
                         float angle/*degree*/
)
{
	// UNIT.06
	render(dc, position, scale, color, angle, { 0.0f, 0.0f }, { static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height) });
}

void SpriteBatch::render(ID3D11DeviceContext* dc, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale)
{
	render(dc, { position },{ scale },{ 1.0f, 1.0f, 1.0f, 1.0f}, 0.0f,
		{ 0.0f, 0.0f }, { static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height) });
}

void SpriteBatch::render(ID3D11DeviceContext* dc, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale,
	DirectX::XMFLOAT4 color,float angle,
	DirectX::XMFLOAT2 tex_pos, DirectX::XMFLOAT2 tex_size
)
{
	//スクリーン(ビューポート)のサイズを取得する
	D3D11_VIEWPORT viewport{}; //ビューポートの寸法の定義　https://docs.microsoft.com/ja-jp/windows/win32/api/d3d11/ns-d3d11-d3d11_viewport
	UINT numViewports{ 1 };
	dc->RSGetViewports(&numViewports, &viewport); //ラスタライザステージにバインドされたビューポートの配列を取得

	//解像度の違いによる差分を補正
	position = { position.x * MAGNI_RESOLUTION_WIDTH, position.y * MAGNI_RESOLUTION_HEIGHT };
	scale = { scale.x * MAGNI_RESOLUTION_WIDTH, scale.y * MAGNI_RESOLUTION_HEIGHT };
	
	//renderメンバ関数の引数から矩形の各頂点の位置を計算する
	// (x0, y0) *----* (x1, y1)
	//			|   /|
	//			|  / |
	//			| /	 |
	//			|/	 |
	// (x2, y2),*----* (x3, y3)


	// left-top
	float x0{ position.x - scale.x };
	float y0{ position.y - scale.y };
	// right-top
	float x1{ position.x + (fabsf(tex_size.x) * scale.x) };
	float y1{ position.y - scale.y };
	// left-bottom
	float x2{ position.x - scale.x };
	float y2{ position.y + (fabsf(tex_size.y) * scale.y) };
	// right-bottom
	float x3{ position.x + (fabsf(tex_size.x)  * scale.x) };
	float y3{ position.y + (fabsf(tex_size.y) * scale.y) };

	



	//回転の中心を矩形の中心点にした場合
	float cx = position.x + static_cast<float>(texture2d_desc.Width) * scale.x * 0.5f;
	float cy = position.y + static_cast<float>(texture2d_desc.Height) * scale.y * 0.5f;
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
	float u0{ tex_pos.x / texture2d_desc.Width };
	float v0{ tex_pos.y / texture2d_desc.Height };

	float u1{ (tex_pos.x + tex_size.x) / texture2d_desc.Width };
	float v1{ (tex_pos.y + tex_size.y) / texture2d_desc.Height };

	vertices.push_back({ {x0, y0, 0}, color, {u0, v0} });
	vertices.push_back({ {x1, y1, 0}, color, {u1, v0} });
	vertices.push_back({ {x2, y2, 0}, color, {u0, v1} });
	vertices.push_back({ {x2, y2, 0}, color, {u0, v1} });
	vertices.push_back({ {x1, y1, 0}, color, {u1, v0} });
	vertices.push_back({ {x3, y3, 0}, color, {u1, v1} });
	
	
}

void SpriteBatch::render(ID3D11DeviceContext* dc, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT2 pivot, DirectX::XMFLOAT4 color, float angle, DirectX::XMFLOAT2 texpos, DirectX::XMFLOAT2 texsize)
{
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	dc->RSGetViewports(&num_viewports, &viewport);

	position = { position.x * MAGNI_RESOLUTION_WIDTH, position.y * MAGNI_RESOLUTION_HEIGHT };
	scale = { scale.x * MAGNI_RESOLUTION_WIDTH, scale.y * MAGNI_RESOLUTION_HEIGHT };

	// renderメンバ関数の引数（dx, dy, dw, dh）から矩形の各頂点の位置（スクリーン座標系）を計算する
	//  (x0, y0) *----* (x1, y1)
	//           |   /|
	//           |  / |
	//           | /  |
	//           |/   |
	//  (x2, y2) *----* (x3, y3)

	// left-top
	float x0{ position.x - (pivot.x * scale.x) };
	float y0{ position.y - (pivot.y * scale.y) };
	// right-top
	float x1{ position.x + ((fabsf(texsize.x) - pivot.x) * scale.x) };
	float y1{ position.y - (pivot.y * scale.y) };
	// left-bottom
	float x2{ position.x - (pivot.x * scale.x) };
	float y2{ position.y + ((fabsf(texsize.y) - pivot.y) * scale.y) };
	// right-bottom
	float x3{ position.x + ((fabsf(texsize.x) - pivot.x) * scale.x) };
	float y3{ position.y + ((fabsf(texsize.y) - pivot.y) * scale.y) };

	//回転の中心を矩形の中心点にした場合
	float cx = position.x;
	float cy = position.y;
	float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
	float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
	rotate(x0, y0, cx, cy, cos, sin);
	rotate(x1, y1, cx, cy, cos, sin);
	rotate(x2, y2, cx, cy, cos, sin);
	rotate(x3, y3, cx, cy, cos, sin);

	// Convert to NDC space
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	float u0{ texpos.x / texture2d_desc.Width };
	float v0{ texpos.y / texture2d_desc.Height };
	float u1{ (texpos.x + texsize.x) / texture2d_desc.Width };
	float v1{ (texpos.y + texsize.y) / texture2d_desc.Height };

	if (scale.x >= 0)
	{
		vertices.push_back({ { x0, y0 , 0 }, { color.x, color.y, color.z, color.w }, { u0, v0 } });
		vertices.push_back({ { x1, y1 , 0 }, { color.x, color.y, color.z, color.w }, { u1, v0 } });
		vertices.push_back({ { x2, y2 , 0 }, { color.x, color.y, color.z, color.w }, { u0, v1 } });
		vertices.push_back({ { x2, y2 , 0 }, { color.x, color.y, color.z, color.w }, { u0, v1 } });
		vertices.push_back({ { x1, y1 , 0 }, { color.x, color.y, color.z, color.w }, { u1, v0 } });
		vertices.push_back({ { x3, y3 , 0 }, { color.x, color.y, color.z, color.w }, { u1, v1 } });
	}
	else
	{
		vertices.push_back({ { x1, y1 , 0 }, { color.x, color.y, color.z, color.w }, { u1, v0 } });
		vertices.push_back({ { x0, y0 , 0 }, { color.x, color.y, color.z, color.w }, { u0, v0 } });
		vertices.push_back({ { x3, y3 , 0 }, { color.x, color.y, color.z, color.w }, { u1, v1 } });
		vertices.push_back({ { x3, y3 , 0 }, { color.x, color.y, color.z, color.w }, { u1, v1 } });
		vertices.push_back({ { x0, y0 , 0 }, { color.x, color.y, color.z, color.w }, { u0, v0 } });
		vertices.push_back({ { x2, y2 , 0 }, { color.x, color.y, color.z, color.w }, { u0, v1 } });
	}

}

void SpriteBatch::begin(ID3D11DeviceContext* dc,
	ID3D11PixelShader* replaced_pixel_shader, ID3D11ShaderResourceView* replaced_shader_resource_view)
{
	vertices.clear();
	dc->VSSetShader(vertex_shader.Get(), nullptr, 0);
	replaced_pixel_shader ? dc->PSSetShader(replaced_pixel_shader, nullptr, 0) : dc->PSSetShader(pixel_shader.Get(), nullptr, 0);
	
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

		dc->PSSetShaderResources(0, 1, &replaced_shader_resource_view);
	}
	else
	{
		dc->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	//デバイスのゼロベースの配列にインデックスを付けて、シェーダーリソースの設定を開始 第2引数:設定するシェーダーリソースの数　第3引数：デバイスに設定するシェーダーリソースビューインターフェイスの配列
	}
	//定数バッファ
	scroll_constants->bind(dc, 2, ::CB_FLAG::PS_VS);

}

void SpriteBatch::end(ID3D11DeviceContext* dc)
{
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	hr = dc->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	size_t vertex_count = vertices.size();
	_ASSERT_EXPR(maxVertices >= vertex_count, "Buffer overflow");
	vertex* data{ reinterpret_cast<vertex*>(mapped_subresource.pData)  };
	if (data != nullptr)
	{
		const vertex* p = vertices.data();
		memcpy_s(data, maxVertices * sizeof(vertex) , p, vertex_count * sizeof(vertex));
	}

	dc->Unmap(vertex_buffer.Get(), 0);

	
	//頂点バッファーのバインド
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	
	//プリミティブタイプおよびデータの順序に関する情報のバインド
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//TRIANGLELISTにしたら3角形１つになった

	//入力レイアウトオブジェクトのバインド
	dc->IASetInputLayout(input_layout.Get());
	//シェーダーのバインド
	
	//プリミティブの描画
	dc->Draw(static_cast<UINT>(vertex_count), 0);

	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	shader_resource_view.Get()->GetResource(resource.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	texture2d->GetDesc(&texture2d_desc);
}

SpriteBatch::~SpriteBatch()
{
	/*vertex_shader->Release();
	pixel_shader->Release();
	input_layout->Release();
	vertex_buffer->Release();
	shader_resource_view->Release();*/

}