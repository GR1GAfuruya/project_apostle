#include "instancing_mesh.h"
#include "resource_manager.h"
#include "noise.h"
#include "texture.h"
#include "user.h"
InstanceMesh::InstanceMesh(Graphics& graphics, const char* fbx_filename,int max_instance)
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
		CD3D11_BUFFER_DESC bufferDesc(sizeof(Instance) * used_instance_count, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		bufferDesc.StructureByteStride = sizeof(Instance);

		
		graphics.get_device().Get()->CreateBuffer(&bufferDesc, nullptr, instance_data.ReleaseAndGetAddressOf());
	}


	CPU_instance_data.reset(new Instance[used_instance_count]);
	//-----------------------------------------------//
	//				定数バッファの生成
	//----------------------------------------------//
	object_constants = std::make_unique<Constants<OBJECT_CONSTANTS>>(graphics.get_device().Get());
	//material_constants = std::make_unique<Constants<MATERIAL_CONSTANTS>>(graphics.get_device().Get());

	//static DirectX::XMFLOAT3 pos = {};
	//for (size_t i = 0; i < used_instance_count; ++i)
	//{
	//	int random = Noise::instance().get_rnd();
	//	float randoom = random % static_cast<int>(10) * 0.5f;
	//	CPU_instance_data[i].position = { 0,5,0 };
	//	//CPU_instance_data[i].position.y = pos.y + randoom;
	//	//CPU_instance_data[i].position.z = pos.z + randoom;
	//	//CPU_instance_data[i].position.y += velocity * elapsed_time;
	//	CPU_instance_data[i].scale = { 1,1,1 };
	//	CPU_instance_data[i].quaternion = { 0,0,0,1 };
	//}
}

void InstanceMesh::update(Graphics& graphics, float elapsed_time)
{
	
	
	for (size_t i = 0; i < used_instance_count; ++i)
	{
		CPU_instance_data[i].position = pos;
		CPU_instance_data[i].scale = { 1,1,1 };
		CPU_instance_data[i].quaternion = Math::rot_quaternion(CPU_instance_data[i].quaternion, DirectX::XMFLOAT3(0,1,0), DirectX::XMConvertToRadians(dir.x),elapsed_time);
		//CPU_instance_data[i].quaternion.w = 1;
	}
	ReplaceBufferContents(graphics, instance_data.Get(), sizeof(Instance) * used_instance_count, CPU_instance_data.get());

}

void InstanceMesh::render(Graphics& graphics)
{

	// Set input assembler state.
	graphics.get_dc().Get()->IASetInputLayout(input_layout.Get());

	// We're rendering a triangle list.
	graphics.get_dc().Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set up the vertex buffers. We have 3 streams:
	// Stream 1 contains per-primitive vertices defining the cubes.
	// Stream 2 contains the per-instance data for scale, position and orientation
	// Stream 3 contains the per-instance data for color.
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

		object_constants->bind(graphics.get_dc().Get(), 0, CB_FLAG::PS_VS);
		int resource_num = 0;
		const int send_texture_num = 1;
		for (auto& s : shader_resources)
		{
			graphics.get_dc().Get()->PSSetShaderResources(20 + resource_num, send_texture_num, s.GetAddressOf());
			resource_num++;
		}
		// シェーダーセット
		graphics.get_dc().Get()->VSSetShader(vertex_shader.Get(), nullptr, 0);
		graphics.get_dc().Get()->PSSetShader(pixel_shader.Get(), nullptr, 0);
		for (const ModelResource::mesh::subset& subset : mesh.subsets)
		{
			//描画命令
			graphics.get_dc().Get()->DrawIndexedInstanced(subset.index_count, used_instance_count, 0, 0, 0);
		}
	}


}

void InstanceMesh::create_pixel_shader(ID3D11Device* device, const char* cso_name)
{
	create_ps_from_cso(device, cso_name, pixel_shader.ReleaseAndGetAddressOf());
}

void InstanceMesh::ReplaceBufferContents(Graphics& graphics,ID3D11Buffer* buffer, size_t bufferSize, const void* data)
{
	D3D11_MAPPED_SUBRESOURCE mapped;

	graphics.get_dc().Get()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, data, bufferSize);
	graphics.get_dc().Get()->Unmap(buffer, 0);
}
void InstanceMesh::register_shader_resource(ID3D11Device* device, const wchar_t* filename)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	load_texture_from_file(device, filename, shader_resource_view.ReleaseAndGetAddressOf(), &texture2d_desc);

	shader_resources.push_back(shader_resource_view);
}