#include "PBR_shader.h"
#include "constant.h"
#include "imgui_include.h"
#include "collision.h"
#include "user.h"
PBRShader::PBRShader(ID3D11Device* device)
{
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
			{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = create_vs_from_cso(device, "shaders\\default_mesh_vs.cso", vertex_shader.ReleaseAndGetAddressOf(),
		input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = create_ps_from_cso(device, "shaders\\PBR_ps.cso", f_pixel_shader.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_ps_from_cso(device, "shaders\\deferred_PBR_ps.cso", d_pixel_shader.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//-----------------------------------------------//
	//				定数バッファの生成
	//----------------------------------------------//
	object_constants = std::make_unique<Constants<OBJECT_CONSTANTS>>(device);
	bone_constants = std::make_unique<Constants<BONE_CONSTANTS>>(device);
	material_constants = std::make_unique<Constants<MATERIAL_CONSTANTS>>(device);
}
void PBRShader::active(ID3D11DeviceContext* immediate_context, RenderType rt)
{
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	//選択されたレンダータイプによってピクセルシェーダーを分岐
	rt == RenderType::Forward ?
		immediate_context->PSSetShader(f_pixel_shader.Get(), nullptr, 0)
		: immediate_context->PSSetShader(d_pixel_shader.Get(), nullptr, 0);

	immediate_context->IASetInputLayout(input_layout.Get());
}


//-----------------------------------------------//
//描画
//----------------------------------------------//
void PBRShader::render(ID3D11DeviceContext* immediate_context, SkeletalMesh* model, const DirectX::XMFLOAT4X4& world)
{
	for (ModelResource::mesh& mesh : model->get_resource()->get_meshes())
	{
		uint32_t stride{ sizeof(ModelResource::vertex) };
		uint32_t offset{ 0 };
		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//アニメーション
		if (&model->anime_param.animation_keyframe && (&model->anime_param.animation_keyframe)->nodes.size() > 0)
		{
			const animation::keyframe::node& mesh_node{ (&model->anime_param.animation_keyframe)->nodes.at(mesh.node_index) };
			XMStoreFloat4x4(&object_constants->data.world, XMLoadFloat4x4(&mesh_node.global_transform) * DirectX::XMLoadFloat4x4(&world));

			const size_t bone_count{ mesh.bind_pose.bones.size() };
			_ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");

			for (size_t bone_index = 0; bone_index < bone_count; ++bone_index)
			{
				const skeleton::bone& bone{ mesh.bind_pose.bones.at(bone_index) };
				// メッシュの位置・姿勢行列をキーフレームから取得
				const animation::keyframe::node& bone_node{ (&model->anime_param.animation_keyframe)->nodes.at(bone.node_index) };
				XMStoreFloat4x4(&bone_constants->data.bone_transforms[bone_index],
					XMLoadFloat4x4(&bone.offset_transform) *
					XMLoadFloat4x4(&bone_node.global_transform) *
					XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.default_global_transform))
				);
			}
			bone_constants->bind(immediate_context, 1, CB_FLAG::VS);
		}
		else
		{
			XMStoreFloat4x4(&object_constants->data.world,
				XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));

			for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
			{
				bone_constants->data.bone_transforms[bone_index] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			}
			bone_constants->bind(immediate_context, 1, CB_FLAG::VS);

		}
		object_constants->data.material_color = { 1.0f,1.0f, 1.0f, 1.0f };
		object_constants->bind(immediate_context, 0, CB_FLAG::VS);
		for (const ModelResource::mesh::subset& subset : mesh.subsets)
		{
			const ModelResource::material& material{ model->get_resource()->materials.at(subset.material_unique_id) };

			// 各種シェーダーなどの初期設定
			shader_resources default_shader_resources;
			default_shader_resources.shader_resource_views[0] = material.shader_resource_views[0].Get();
			default_shader_resources.shader_resource_views[1] = material.shader_resource_views[1].Get();
			default_shader_resources.shader_resource_views[2] = material.shader_resource_views[2].Get();
			default_shader_resources.shader_resource_views[3] = material.shader_resource_views[3].Get();
			default_shader_resources.shader_resource_views[4] = material.shader_resource_views[4].Get();
			default_shader_resources.shader_resource_views[5] = material.shader_resource_views[5].Get();

			material_constants->data = default_shader_resources.material_data;
			material_constants->bind(immediate_context, 2, CB_FLAG::PS_VS);
			immediate_context->PSSetShaderResources(0, 1, &default_shader_resources.shader_resource_views[0]);
			immediate_context->PSSetShaderResources(1, 1, &default_shader_resources.shader_resource_views[1]);
			immediate_context->PSSetShaderResources(2, 1, &default_shader_resources.shader_resource_views[2]);
			immediate_context->PSSetShaderResources(3, 1, &default_shader_resources.shader_resource_views[3]);
			immediate_context->PSSetShaderResources(4, 1, &default_shader_resources.shader_resource_views[4]);
			immediate_context->PSSetShaderResources(5, 1, &default_shader_resources.shader_resource_views[5]);

			immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
		}
	}
}

void PBRShader::render(ID3D11DeviceContext* immediate_context, SkeletalMesh* model, DirectX::XMFLOAT4X4 camera_view, DirectX::XMFLOAT4X4 camara_proj, const DirectX::XMFLOAT4X4& world)
{
	DirectX::XMFLOAT3 world_bounding_box_min;
	DirectX::XMFLOAT3 world_bounding_box_max;


	for (ModelResource::mesh& mesh : model->get_resource()->get_meshes())
	{
		uint32_t stride{ sizeof(ModelResource::vertex) };
		uint32_t offset{ 0 };
		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//アニメーション
		if (&model->anime_param.animation_keyframe && (&model->anime_param.animation_keyframe)->nodes.size() > 0)
		{
			const animation::keyframe::node& mesh_node{ (&model->anime_param.animation_keyframe)->nodes.at(mesh.node_index) };
			XMStoreFloat4x4(&object_constants->data.world, XMLoadFloat4x4(&mesh_node.global_transform) * DirectX::XMLoadFloat4x4(&world));

			const size_t bone_count{ mesh.bind_pose.bones.size() };
			_ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");

			DirectX::XMVECTOR blended_min = {}, blended_max = {};

			for (size_t bone_index = 0; bone_index < bone_count; ++bone_index)
			{
				const skeleton::bone& bone{ mesh.bind_pose.bones.at(bone_index) };
				// メッシュの位置・姿勢行列をキーフレームから取得
				const animation::keyframe::node& bone_node{ (&model->anime_param.animation_keyframe)->nodes.at(bone.node_index) };
				XMStoreFloat4x4(&bone_constants->data.bone_transforms[bone_index],
					XMLoadFloat4x4(&bone.offset_transform) *
					XMLoadFloat4x4(&bone_node.global_transform) *
					XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.default_global_transform))
				);

				using namespace DirectX;

				blended_min += (1.0f / bone_count)/*重み付け*/ * DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&mesh.bounding_box[0]),
					XMLoadFloat4x4(&bone_constants->data.bone_transforms[bone_index]));
				
				blended_max += (1.0f / bone_count)/*重み付け*/ * DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&mesh.bounding_box[1]),
					XMLoadFloat4x4(&bone_constants->data.bone_transforms[bone_index]));
			}
			bone_constants->bind(immediate_context, 1, CB_FLAG::VS);

			DirectX::XMVECTOR world_min = DirectX::XMVector3TransformCoord(blended_min, XMLoadFloat4x4(&object_constants->data.world));
			DirectX::XMStoreFloat3(&world_bounding_box_min, world_min);

			DirectX::XMVECTOR world_max = DirectX::XMVector3TransformCoord(blended_max, XMLoadFloat4x4(&object_constants->data.world));
			DirectX::XMStoreFloat3(&world_bounding_box_max, world_max);
		}
		else
		{
			XMStoreFloat4x4(&object_constants->data.world,
				XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));

			for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
			{
				bone_constants->data.bone_transforms[bone_index] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			}
			bone_constants->bind(immediate_context, 1, CB_FLAG::VS);


			DirectX::XMVECTOR world_min = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&mesh.bounding_box[0]),
				XMLoadFloat4x4(&object_constants->data.world));
			DirectX::XMStoreFloat3(&world_bounding_box_min, world_min);

			DirectX::XMVECTOR world_max = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&mesh.bounding_box[1]),
				XMLoadFloat4x4(&object_constants->data.world));
			DirectX::XMStoreFloat3(&world_bounding_box_max, world_max);
		}



		if (!Collision::frustum_vs_cuboid(camera_view, camara_proj, world_bounding_box_min, world_bounding_box_max))
		{
			continue;
		}


		object_constants->data.material_color = { 1.0f,1.0f, 1.0f, 1.0f };
		object_constants->bind(immediate_context, 0, CB_FLAG::VS);
		for (const ModelResource::mesh::subset& subset : mesh.subsets)
		{
			const ModelResource::material& material{ model->get_resource()->materials.at(subset.material_unique_id) };

			// 各種シェーダーなどの初期設定
			shader_resources default_shader_resources;
			default_shader_resources.shader_resource_views[0] = material.shader_resource_views[0].Get();
			default_shader_resources.shader_resource_views[1] = material.shader_resource_views[1].Get();
			default_shader_resources.shader_resource_views[2] = material.shader_resource_views[2].Get();
			default_shader_resources.shader_resource_views[3] = material.shader_resource_views[3].Get();
			default_shader_resources.shader_resource_views[4] = material.shader_resource_views[4].Get();
			default_shader_resources.shader_resource_views[5] = material.shader_resource_views[5].Get();

			material_constants->data = default_shader_resources.material_data;
			material_constants->bind(immediate_context, 2, CB_FLAG::PS_VS);
			immediate_context->PSSetShaderResources(0, 1, &default_shader_resources.shader_resource_views[0]);
			immediate_context->PSSetShaderResources(1, 1, &default_shader_resources.shader_resource_views[1]);
			immediate_context->PSSetShaderResources(2, 1, &default_shader_resources.shader_resource_views[2]);
			immediate_context->PSSetShaderResources(3, 1, &default_shader_resources.shader_resource_views[3]);
			immediate_context->PSSetShaderResources(4, 1, &default_shader_resources.shader_resource_views[4]);
			immediate_context->PSSetShaderResources(5, 1, &default_shader_resources.shader_resource_views[5]);

			immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
		}
	}
}
