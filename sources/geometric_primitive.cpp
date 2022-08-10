#include "geometric_primitive.h"
GeometricPrimitive::GeometricPrimitive(ID3D11Device* device)
{

	//サイズが1.0の正立方体データを作成する（重心を原点にする)。正立方体のコントロールポイント数は8個、
	//1つのコントロールポイントの位置には法線の向きが違う頂点が3個あるので頂点情報の総数は8x3=24個、
	// 頂点情報配列（vertices）にすべて頂点の位置・法線情報を格納する。
	//(表面と裏面の頂点が８でそれが3セット)

	vertex vertices[24]
	{
		//  上面																					1
		vertices[0].position = {-0.5f, 0.5f, 0.5f},					//        	   0/----------/ |13
		vertices[0].normal = {0, 1, 0},								//		       /		  /	 |
		vertices[1].position = {0.5f, 0.5f, 0.5f},					//		     2|----------| 3 |	   
		vertices[1].normal = {0, 1, 0},								//		     4|          |5  |  15    			
		vertices[2].position = {-0.5f, 0.5f, -0.5f},				//	          |          | 	/         	
		vertices[2].normal = {0, 1, 0},								//	          |          | /           		
		vertices[3].position = {0.5f, 0.5f, -0.5f},					//            |----------|/  14          
		vertices[3].normal = {0, 1, 0},								//			6			7                 

		//前面
		vertices[4].position = {-0.5f, 0.5f, -0.5f},
		vertices[4].normal = {0, 0, -1},
		vertices[5].position = {0.5f, 0.5f, -0.5f},
		vertices[5].normal = {0, 0, -1},
		vertices[6].position = {-0.5f, -0.5f, -0.5f},
		vertices[6].normal = {0, 0, -1},
		vertices[7].position = {0.5f, -0.5f, -0.5f},
		vertices[7].normal = {0, 0, -1},

		//下面
		vertices[8].position = {-0.5f, -0.5f, -0.5f},
		vertices[8].normal = {0, -1, 0},
		vertices[9].position = {0.5f, -0.5f, -0.5f},
		vertices[9].normal = {0, -1, 0},
		vertices[10].position = {-0.5f, -0.5f, 0.5f},
		vertices[10].normal = {0, -1, 0},
		vertices[11].position = {0.5f, -0.5f, 0.5f},
		vertices[11].normal = {0, -1, 0},

		//右面
		vertices[12].position = {0.5f, 0.5f, -0.5f},
		vertices[12].normal = {1, 0, 0},
		vertices[13].position = {0.5f, 0.5f, 0.5f},
		vertices[13].normal = {1, 0, 0},
		vertices[14].position = {0.5f, -0.5f, -0.5f},
		vertices[14].normal = {1, 0, 0},
		vertices[15].position = {0.5f, -0.5f, 0.5f},
		vertices[15].normal = {1, 0, 0},

		//後面
		vertices[16].position = {0.5f, 0.5f, 0.5f},
		vertices[16].normal = {0, 0, 1},
		vertices[17].position = {-0.5f, 0.5f, 0.5f},
		vertices[17].normal = {0, 0, 1},
		vertices[18].position = {0.5f, -0.5f, 0.5f},
		vertices[18].normal = {0, 0, 1},
		vertices[19].position = {-0.5f, -0.5f, 0.5f},
		vertices[19].normal = {0, 0, 1},

		//左面
		vertices[20].position = {-0.5f, 0.5f, 0.5f},
		vertices[20].normal = {-1, 0, 0},
		vertices[21].position = {-0.5f, 0.5f, -0.5f},
		vertices[21].normal = {-1, 0, 0},
		vertices[22].position = {-0.5f, -0.5f, 0.5f},
		vertices[22].normal = {-1, 0, 0},
		vertices[23].position = {-0.5f, -0.5f, -0.5f},
		vertices[23].normal = {-1, 0, 0},
	};

	// 正立方体は6面持ち、1つの面は2つの3角形ポリゴンで構成されるので3角形ポリゴンの総数は6x2=12個、
	// 正立方体を描画するために12回の3角形ポリゴン描画が必要、よって参照される頂点情報は12x3=36回、
	// 3角形ポリゴンが参照する頂点情報のインデックス（頂点番号）を描画順に配列（indices）に格納する。
	// 時計回りが表面になるように格納すること。
	uint32_t indices[36]
	{
	indices[0] = 0,		indices[1] = 1,		indices[2] = 2,
	indices[3] = 1,		indices[4] = 3,		indices[5] = 2,
	indices[6] = 4,		indices[7] = 5,		indices[8] = 6,
	indices[9] = 5,		indices[10] = 7,	indices[11] = 6,
	indices[12] = 8,	indices[13] = 9,	indices[14] = 10,
	indices[15] = 9,	indices[16] = 11,	indices[17] = 10,
	indices[18] = 12,	indices[19] = 13,	indices[20] = 14,
	indices[21] = 13,	indices[22] = 15,	indices[23] = 14,
	indices[24] = 16,	indices[25] = 17,	indices[26] = 18,
	indices[27] = 17,	indices[28] = 19,	indices[29] = 18,
	indices[30] = 20,	indices[31] = 21,	indices[32] = 22,
	indices[33] = 21,	indices[34] = 23,	indices[35] = 22,
	};


	create_com_buffers(device, vertices, 24, indices, 36);

	HRESULT hr{ S_OK };

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	create_vs_from_cso(device, "shaders/geometric_primitive_vs.cso", vertex_shader.GetAddressOf(),
		input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	create_ps_from_cso(device, "shaders/geometric_primitive_ps.cso", pixel_shader.GetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(constans);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


}

void GeometricPrimitive::create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
	uint32_t* indices, size_t index_count)
{
	HRESULT hr{ S_OK };
	D3D11_BUFFER_DESC buffer_desc{};
	D3D11_SUBRESOURCE_DATA subresource_data{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertex_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * index_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subresource_data.pSysMem = indices;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void GeometricPrimitive::render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color)
{
	uint32_t stride{ sizeof(vertex) };
	uint32_t offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediate_context->IASetInputLayout(input_layout.Get());

	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	constans data{ world, material_color };
	immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
	immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	index_buffer->GetDesc(&buffer_desc);
	immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);




}

