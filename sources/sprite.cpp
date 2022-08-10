#include "sprite.h"
#include "texture.h"
#include <sstream>


Sprite::Sprite(ID3D11Device* device, const wchar_t* filename): scroll_data()
{
	HRESULT hr{S_OK};

	//頂点情報のセット
	vertex vertices[]
	{
		{{-1.0, +1.0, 0}, {1, 1, 1, 1}, {0, 0}}, //頂点左上
		{{+1.0, +1.0, 0}, {1, 1, 1, 1}, {1, 0}}, //頂点右上
		{{-1.0, -1.0, 0}, {1, 1, 1, 1}, {0, 1}}, //頂点左下
		{{+1.0, -1.0, 0}, {1, 1, 1, 1}, {1, 1}}, //頂点右下
	};

	//頂点バッファオブジェクトの生成
	D3D11_BUFFER_DESC bufferDesc{}; //	https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee416048(v=vs.85)
	bufferDesc.ByteWidth = sizeof(vertices); //バッファーのサイズ
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //バッファーで想定されている読み込みおよび書き込みの方法を識別。　更新頻度は重要な要素
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //バッファーをどのようにパイプラインにバインドするかを識別		フラグは論理和で組み合わせられる
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPUアクセスのフラグ	必要ない場合は０
	bufferDesc.MiscFlags = 0;
	//その他のフラグ（https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee416267(v=vs.85)　を参照）
	bufferDesc.StructureByteStride = 0; //構造体が構造化バッファーを表す場合、その構造体のサイズ (バイト単位) です。
	D3D11_SUBRESOURCE_DATA subresourceData{};
	//https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee416284(v=vs.85)
	subresourceData.pSysMem = vertices; //初期化データへのポインター
	subresourceData.SysMemPitch = 0; //テクスチャーにある1本の線の先端から隣の線までの距離
	subresourceData.SysMemSlicePitch = 0; //1つの深度レベルの先端から隣の深度レベルまでの距離

	hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertex_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//頂点シェーダーオブジェクトの生成
	const char* cso_name{"shaders/sprite_vs.cso"};

	FILE* fp{};
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long csoSz{ftell(fp)};
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> csoData{std::make_unique<unsigned char[]>(csoSz)};
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);


	hr = device->CreateVertexShader(csoData.get(), csoSz, nullptr, vertex_shader.GetAddressOf());
	//https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createvertexshader
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//定数バッファ
	scroll_constants = std::make_unique<Constants<SCROLL_CONSTANTS>>(device);

	//入力レイアウトオブジェクトの生成
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		//https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_input_element_desc
		{
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			},
		};
	hr = device->CreateInputLayout(inputElementDesc, _countof(inputElementDesc),
	                               csoData.get(), csoSz, input_layout.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//ピクセルシェーダーオブジェクトの生成
	const char* cso_pname{"shaders/sprite_ps.cso"};

	FILE* fpx{};
	fopen_s(&fpx, cso_pname, "rb");
	_ASSERT_EXPR_A(fpx, "CSO File not found");

	fseek(fpx, 0, SEEK_END);
	long csoSzp{ftell(fpx)};
	fseek(fpx, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> csoPData{std::make_unique<unsigned char[]>(csoSzp)};
	fread(csoPData.get(), csoSzp, 1, fpx);
	fclose(fpx);


	hr = device->CreatePixelShader(csoPData.get(), csoSzp, nullptr, pixel_shader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//画像ファイルのロードとシェーダーリソースビューオブジェクト(ID3D11ShaderResourceView)の生成

	hr = load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(), &texture2dDesc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

Sprite::Sprite(ID3D11Device* device)
{
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




void Sprite::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy,		//矩形の左上の座標(スクリーン座標系)
	float dw, float dh,		//矩形のサイズ(スクリーン座標系)
	float r, float g, float b, float a,
	float angle/*degree*/,
	float sx, float sy,float sw, float sh
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

	////切り抜き
	
	float sx0{ sx };
	float sy0{sy };
	//right-top
	float sx1{ sx + sw };
	float sy1{ sy };
	//left-bottom
	float sx2{ sx };
	float sy2{ sy + sh };
	//right-bottom
	float sx3{ sx + sw };
	float sy3{ sy + sh };



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
	sx0 = sx0 / texture2dDesc.Width;
	sy0 = sy0 / texture2dDesc.Height;

	sx1 =  sx1  / texture2dDesc.Width;
	sy1 =  sy1  / texture2dDesc.Height;

	sx2 =  sx2 / texture2dDesc.Width;
	sy2 =  sy2 / texture2dDesc.Height;

	sx3 =  sx3/ texture2dDesc.Width;
	sy3 = sy3  / texture2dDesc.Height;


	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	vertex* vertices{ reinterpret_cast<vertex*>(mappedSubresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position = { x0, y0, 0 };
		vertices[1].position = { x1, y1, 0 };
		vertices[2].position = { x2, y2, 0 };
		vertices[3].position = { x3, y3, 0 };
		//vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r, g, b, a };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r, g, b, a };

		vertices[0].texcoord = { sx0, sy0 };		//画像左上の座標
		vertices[1].texcoord = { sx1, sy1 };
		vertices[2].texcoord = { sx2, sy2 };
		vertices[3].texcoord = { sx3, sy3 };

	}

	immediate_context->Unmap(vertex_buffer.Get(), 0);

	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	//デバイスのゼロベースの配列にインデックスを付けて、シェーダーリソースの設定を開始 第2引数:設定するシェーダーリソースの数　第3引数：デバイスに設定するシェーダーリソースビューインターフェイスの配列
	//頂点バッファーのバインド
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	//プリミティブタイプおよびデータの順序に関する情報のバインド
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	//TRIANGLELISTにしたら3角形１つになった

	//入力レイアウトオブジェクトのバインド
	immediate_context->IASetInputLayout(input_layout.Get());
	//シェーダーのバインド
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);	//https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-VSSetShader
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	//プリミティブの描画
	immediate_context->Draw(4, 0);
}

void Sprite::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy,		//矩形の左上の座標(スクリーン座標系)
	float dw, float dh		//矩形のサイズ(スクリーン座標系)
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



	

	////スクリーン座標系からNDCへの座標返還を行う
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;


	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	vertex* vertices{ reinterpret_cast<vertex*>(mappedSubresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position = { x0, y0, 0 };
		vertices[1].position = { x1, y1, 0 };
		vertices[2].position = { x2, y2, 0 };
		vertices[3].position = { x3, y3, 0 };
		//vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r, g, b, a };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { 1.0f, 1.0f, 1.0f, 1.0f };

		vertices[0].texcoord = { 0, 0 };		//画像左上の座標
		vertices[1].texcoord = { 1, 0 };
		vertices[2].texcoord = { 0, 1 };
		vertices[3].texcoord = { 1, 1 };

	}

	immediate_context->Unmap(vertex_buffer.Get(), 0);

	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	//デバイスのゼロベースの配列にインデックスを付けて、シェーダーリソースの設定を開始 第2引数:設定するシェーダーリソースの数　第3引数：デバイスに設定するシェーダーリソースビューインターフェイスの配列
	//頂点バッファーのバインド
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	//プリミティブタイプおよびデータの順序に関する情報のバインド
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	//TRIANGLELISTにしたら3角形１つになった

	//入力レイアウトオブジェクトのバインド
	immediate_context->IASetInputLayout(input_layout.Get());
	//シェーダーのバインド
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);	//https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-VSSetShader
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	//プリミティブの描画
	immediate_context->Draw(4, 0);
}

void Sprite::textout(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a)
{
	float sw = static_cast<float>(texture2dDesc.Width / 16);
	float sh = static_cast<float>(texture2dDesc.Height / 16);
	float carriage = 0;
	for (const char c : s)
	{
		render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0,
			sw * (c & 0x0F), sh * (c >> 4), sw, sh);
		carriage += w;
	}
}

Sprite::~Sprite()
{
	/*vertex_shader->Release();
	pixel_shader->Release();
	input_layout->Release();
	vertex_buffer->Release();
	shader_resource_view->Release();*/
	
}