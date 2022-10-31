#include "instancing_mesh.h"
#include "resource_manager.h"
//==============================================================
// 
// コンストラクタ
// 
//==============================================================
InstanceMesh::InstanceMesh(Graphics& graphics, const char* fbx_filename, const int max_instance)
{
	model = ResourceManager::instance().load_model_resource(graphics.get_device().Get(), fbx_filename, true, 60.0f);

	used_instance_count = max_instance;
	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "I_ROTATION", 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			0,D3D11_INPUT_PER_INSTANCE_DATA,    1 },  // Instance rotation quaternion
			{ "I_POSITION", 0,DXGI_FORMAT_R32G32B32_FLOAT, 1,
			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,    1 },  
			{ "I_SCALE", 0,DXGI_FORMAT_R32G32B32_FLOAT, 1,
			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,    1 },  
	};

	hr = create_vs_from_cso(graphics.get_device().Get(), "shaders\\instancing_mesh_vs.cso", vertex_shader.ReleaseAndGetAddressOf(),
		input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	{
		CD3D11_BUFFER_DESC bufferDesc(sizeof(Instance) * max_instance, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		bufferDesc.StructureByteStride = sizeof(Instance);

		graphics.get_device().Get()->CreateBuffer(&bufferDesc, nullptr, instance_data.ReleaseAndGetAddressOf());
	}

	
	CPU_instance_data.reset(new Instance[max_instance]);
	//-----------------------------------------------//
	//				定数バッファの生成
	//----------------------------------------------//
	object_constants = std::make_unique<Constants<OBJECT_CONSTANTS>>(graphics.get_device().Get());
	
}
//==============================================================
// 
// 描画
// 
//==============================================================
void InstanceMesh::render(Graphics& graphics)
{

	ReplaceBufferContents(graphics, instance_data.Get(), sizeof(Instance) * used_instance_count, CPU_instance_data.get());
	graphics.get_dc().Get()->IASetInputLayout(input_layout.Get());
	graphics.get_dc().Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (ModelResource::mesh& mesh : model->get_meshes())
	{
		UINT Strides[] = { sizeof(ModelResource::vertex), sizeof(Instance) };
		UINT Offsets[] = { 0, 0 };
		
		ID3D11Buffer* Buffers[] = { mesh.vertex_buffer.Get(), instance_data.Get()};
		graphics.get_dc().Get()->IASetVertexBuffers(0, _countof(Strides), Buffers, Strides, Offsets);
		

		// The per-instance data is referenced by index...
		graphics.get_dc().Get()->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//定数バッファにメッシュのglobal_transformを送る
		XMStoreFloat4x4(&object_constants->data.global_transform,
			XMLoadFloat4x4(&mesh.default_global_transform));

		object_constants->bind(graphics.get_dc().Get(), 0, CB_FLAG::VS);
		int resource_num = 0;
		const int send_texture_num = 1;
		for (auto& s : shader_resources)
		{
			graphics.get_dc().Get()->PSSetShaderResources(20 + resource_num, send_texture_num, s.GetAddressOf());
			resource_num++;
		}
		
		for (const ModelResource::mesh::subset& subset : mesh.subsets)
		{
			//描画命令
			graphics.get_dc().Get()->DrawIndexedInstanced(subset.index_count, used_instance_count, 0, 0, 0);
		}
	}
}
//==============================================================
// 
// シェーダーのセット
// 
//==============================================================
void InstanceMesh::active(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* alter_pixcel_shader)
{
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(alter_pixcel_shader, nullptr, 0);
}
//==============================================================
// 
// 位置、姿勢、大きさをバッファに格納
// 
//==============================================================
void InstanceMesh::ReplaceBufferContents(Graphics& graphics,ID3D11Buffer* buffer, size_t bufferSize, const void* data)
{
	D3D11_MAPPED_SUBRESOURCE mapped;

	graphics.get_dc().Get()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, data, bufferSize);
	graphics.get_dc().Get()->Unmap(buffer, 0);
}
