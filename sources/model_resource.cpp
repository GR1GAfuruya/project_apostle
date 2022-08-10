#include "model_resource.h"


#include "misc.h"
#include <fstream>
#include <sstream>
#include <functional>
#include <filesystem>


#include "texture.h"
//----------------------------------------------
//
//    inline関数
//
//----------------------------------------------
inline XMFLOAT4X4 to_xmfloat4x4(const FbxAMatrix& fbxamatrix)
{
	XMFLOAT4X4 xmfloat4x4;
	for (int row = 0; row < 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
	return xmfloat4x4;
}
inline XMFLOAT3 to_xmfloat3(const FbxDouble3& fbxdouble3)
{
	XMFLOAT3 xmfloat3;
	xmfloat3.x = static_cast<float>(fbxdouble3[0]);
	xmfloat3.y = static_cast<float>(fbxdouble3[1]);
	xmfloat3.z = static_cast<float>(fbxdouble3[2]);
	return xmfloat3;
}
inline XMFLOAT4 to_xmfloat4(const FbxDouble4& fbxdouble4)
{
	XMFLOAT4 xmfloat4;
	xmfloat4.x = static_cast<float>(fbxdouble4[0]);
	xmfloat4.y = static_cast<float>(fbxdouble4[1]);
	xmfloat4.z = static_cast<float>(fbxdouble4[2]);
	xmfloat4.w = static_cast<float>(fbxdouble4[3]);
	return xmfloat4;
}
//-----------------------------------------------//
//　ボーン影響度を表現する構造体
//----------------------------------------------//
struct bone_influence
{
	uint32_t bone_index;
	float bone_weight; //1本のボーンがある一つの頂点に与える影響力
};
using bone_influences_per_control_point = std::vector<bone_influence>;

//-----------------------------------------------//
//　ボーン影響度の抽出　　http://marupeke296.com/FBX_No9_Bone.html
//----------------------------------------------//
void fetch_bone_influences(const FbxMesh* fbx_mesh,
	std::vector<bone_influences_per_control_point>& bone_influences)
{
	const int control_points_count{ fbx_mesh->GetControlPointsCount() };
	bone_influences.resize(control_points_count);

	const int skin_count{ fbx_mesh->GetDeformerCount() }; //スキンの数を取得
	for (int skin_index = 0; skin_index < skin_count; ++skin_index)
	{
		const FbxSkin* fbx_skin
		{ static_cast<FbxSkin*>(fbx_mesh->GetDeformer(skin_index, FbxDeformer::eSkin)) }; //skin_index番目のスキンを取得

		//取り出したスキンには複数のクラスタが含まれており、クラスタはスキンに作用する１本のボーンの情報を管理している
		const int cluster_count{ fbx_skin->GetClusterCount() }; //クラスタの数を取得
		for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
		{
			const FbxCluster* fbx_cluster{ fbx_skin->GetCluster(cluster_index) };// cluster_index番目のクラスタを取得

			const int control_point_indices_count{ fbx_cluster->GetControlPointIndicesCount() };
			for (int control_point_indices_index = 0; control_point_indices_index < control_point_indices_count;
				++control_point_indices_index)
			{
				int control_point_index{ fbx_cluster->GetControlPointIndices()[control_point_indices_index] };
				double control_point_weight
				{ fbx_cluster->GetControlPointWeights()[control_point_indices_index] };
				bone_influence& bone_influence{ bone_influences.at(control_point_index).emplace_back() };
				bone_influence.bone_index = static_cast<uint32_t>(cluster_index); //bone_influenceに頂点インデックスを代入
				bone_influence.bone_weight = static_cast<float>(control_point_weight);//bone_influenceにウェイトを代入
			}
		}
	}
}


//-----------------------------------------------//
// FBX SDKの実装　https://yttm-work.jp/model_render/model_render_0007.html //
//----------------------------------------------//		
ModelResource::ModelResource(ID3D11Device* device, const char* fbx_filename, float sampling_rate, bool triangulate)
{
	std::filesystem::path cereal_filename(fbx_filename);
	cereal_filename.replace_extension("cereal");
	if (std::filesystem::exists(cereal_filename.c_str()))
	{
		std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(scene_view, meshes, materials, animation_clips);
	}
	else
	{
		FbxManager* fbx_manager{ FbxManager::Create() };
		FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };  //Importerによって、分解されたFbxのデータを保存するためのクラス

		FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };//FbxファイルをImportするためのパーサークラス
		bool import_status{ false };
		import_status = fbx_importer->Initialize(fbx_filename); //指定したファイルを開いて、初期化する
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

		import_status = fbx_importer->Import(fbx_scene); //シーンにインポート
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

		//モデルの三角形化 https://yttm-work.jp/model_render/model_render_0009.html
		FbxGeometryConverter fbx_converter(fbx_manager);
		if (triangulate)
		{
			fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*lagacy*/);
			fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
		}


		std::function<void(FbxNode*)> traverse{
			[&](FbxNode* fbx_node)
			{
			scene::node& node{scene_view.nodes.emplace_back()};
			node.attribute = fbx_node->GetNodeAttribute() ?
				fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown; //FbxNodeAttributeは様々なデータの基底となっているクラス
			node.name = fbx_node->GetName();
			node.unique_id = fbx_node->GetUniqueID();
			node.parent_index = scene_view.indexof(fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0);
			for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
			{
				traverse(fbx_node->GetChild(child_index));
			}
			}
		};
		traverse(fbx_scene->GetRootNode());

		//メッシュ取り出し
		fetch_meshes(fbx_scene, meshes, sampling_rate);

		//マテリアル取り出し 
		fetch_materials(fbx_filename, fbx_scene, materials);

		//アニメーション取り出し
		fetch_animations(fbx_scene, animation_clips, sampling_rate);

#if 0
		for (const scene::node& node : scene_view.nodes)
		{
			FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
			// Display node data in the output window as debug
			std::string node_name = fbx_node->GetName();
			uint64_t uid = fbx_node->GetUniqueID();
			uint64_t parent_uid = fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0;
			int32_t type = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : 0;

			std::stringstream debug_string;
			debug_string << node_name << ":" << uid << ":" << parent_uid << ":" << type << "\n";
			OutputDebugStringA(debug_string.str().c_str());
		}
#endif
		fbx_manager->Destroy();

		std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive serialization(ofs);
		serialization(scene_view, meshes, materials, animation_clips);

	}
	create_com_objects(device, fbx_filename);

}

//-----------------------------------------------//
//COMオブジェクトの生成
//----------------------------------------------//
void ModelResource::create_com_objects(ID3D11Device* device, const char* fbx_filename)
{
	for (mesh& mesh : meshes)
	{
		HRESULT hr{ S_OK };
		D3D11_BUFFER_DESC buffer_desc{};
		D3D11_SUBRESOURCE_DATA subresource_data{};
		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * mesh.vertices.size());
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = mesh.vertices.data();
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&buffer_desc, &subresource_data,
			mesh.vertex_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subresource_data.pSysMem = mesh.indices.data();
		hr = device->CreateBuffer(&buffer_desc, &subresource_data,
			mesh.index_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#if 0
		mesh.vertices.clear();
		mesh.indices.clear();
#endif
	}

	for (std::unordered_map<uint64_t, material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator)
	{
		for (size_t texture_index = 0; texture_index < material::TEXTURE_COUNT; ++texture_index)
		{
			if (iterator->second.texture_filenames[texture_index].size() > 0)
			{
				std::filesystem::path path(fbx_filename);
				path.replace_filename(iterator->second.texture_filenames[texture_index]);
				D3D11_TEXTURE2D_DESC texture2d_desc;
				load_texture_from_file(device, path.c_str(),
					iterator->second.shader_resource_views[texture_index].GetAddressOf(), &texture2d_desc);
			}
			else
			{
				make_dummy_texture(device,
					iterator->second.shader_resource_views[texture_index].GetAddressOf(),
					texture_index == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16);
			}
		}
	}



}


//-----------------------------------------------//
//FBXのマテリアル情報の取り出し　https://yttm-work.jp/model_render/model_render_0011.html
//----------------------------------------------//
void ModelResource::fetch_materials(const char* fbx_filename, FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials)
{
	const size_t node_count{ scene_view.nodes.size() };
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		const scene::node& node{ scene_view.nodes.at(node_index) };
		const FbxNode* fbx_node{ FindFbxNodeByUniqueId(fbx_scene, node.unique_id) }; //UEのアセットに対応
		//const FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };

		const int material_count{ fbx_node->GetMaterialCount() };
		for (int material_index = 0; material_index < material_count; ++material_index)
		{
			const FbxSurfaceMaterial* fbx_material{ fbx_node->GetMaterial(material_index) };

			material material;
			material.name = fbx_material->GetName();
			material.unique_id = fbx_material->GetUniqueID();
			FbxProperty fbx_property;							//マテリアル情報探しスタート
			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse); //指定されたプロパティを見つける
			if (fbx_property.IsValid()) //プロパティの有効、無効判定
			{
				const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };  //Get: テンプレートで指定した型を返す
				material.diffuse.x = static_cast<float>(color[0]);
				material.diffuse.y = static_cast<float>(color[1]);
				material.diffuse.z = static_cast<float>(color[2]);
				material.diffuse.w = 1.0f;

				const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
				material.texture_filenames[0] =
					fbx_texture ? fbx_texture->GetRelativeFileName() : "";
			}
			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);
			if (fbx_property.IsValid())
			{
				const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
				material.ambient.x = static_cast<float>(color[0]);
				material.ambient.y = static_cast<float>(color[1]);
				material.ambient.z = static_cast<float>(color[2]);
				material.ambient.w = 1.0f;
			}
			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sSpecular);
			if (fbx_property.IsValid())
			{
				const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
				material.specular.x = static_cast<float>(color[0]);
				material.specular.y = static_cast<float>(color[1]);
				material.specular.z = static_cast<float>(color[2]);
				material.specular.w = 1.0f;
			}
			// 法線マップのファイル名
			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sNormalMap);
			if (fbx_property.IsValid())
			{
				std::string color_map_filename = material.texture_filenames[0];
				std::string normal_map_filename = "";
				if (color_map_filename.find("Color.png") != std::string::npos)
				{
					normal_map_filename = color_map_filename.erase(color_map_filename.find("Color.png")) + "N.png";
				}
				std::filesystem::path fbm_filename(fbx_filename);
				fbm_filename.remove_filename();
				std::string s = fbm_filename.string() + normal_map_filename;
				if (std::filesystem::exists(s.c_str()))
				{
					material.texture_filenames[1] = normal_map_filename;
				}
				else
				{
					const FbxFileTexture* file_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
					material.texture_filenames[1] = file_texture ? file_texture->GetRelativeFileName() : "";
				}
			}
			// metal map
			{
				std::string color_map_filename = material.texture_filenames[0];
				std::string metallic_map_filename = "";
				if (color_map_filename.find("Color.png") != std::string::npos)
				{
					metallic_map_filename = color_map_filename.erase(color_map_filename.find("Color.png")) + "M.tga";
				}
				std::filesystem::path fbm_filename(fbx_filename);
				fbm_filename.remove_filename();
				std::string s = fbm_filename.string() + metallic_map_filename;
				if (std::filesystem::exists(s.c_str()))
				{
					material.texture_filenames[2] = metallic_map_filename;
				}
				else
				{
					material.texture_filenames[2] = "";
				}
			}
			// Roughness map
			{
				std::string color_map_filename = material.texture_filenames[0];
				std::string roughness_map_filename = "";
				if (color_map_filename.find("Color.png") != std::string::npos)
				{
					roughness_map_filename = color_map_filename.erase(color_map_filename.find("Color.png")) + "M.tga";
				}
				std::filesystem::path fbm_filename(fbx_filename);
				fbm_filename.remove_filename();
				std::string s = fbm_filename.string() + roughness_map_filename;
				if (std::filesystem::exists(s.c_str()))
				{
					material.texture_filenames[3] = roughness_map_filename;
				}
				else
				{
					material.texture_filenames[3] = "";
				}
			}
			// AO map
			{
				std::string color_map_filename = material.texture_filenames[0];
				std::string ao_map_filename = "";
				if (color_map_filename.find("Color.png") != std::string::npos)
				{
					ao_map_filename = color_map_filename.erase(color_map_filename.find("Color.png")) + "Ao.png";
				}
				std::filesystem::path fbm_filename(fbx_filename);
				fbm_filename.remove_filename();
				std::string s = fbm_filename.string() + ao_map_filename;
				if (std::filesystem::exists(s.c_str()))
				{
					material.texture_filenames[4] = ao_map_filename;
				}
				else
				{
					material.texture_filenames[4] = "";
				}
			}
			// Emissive map
			{
				std::string color_map_filename = material.texture_filenames[0];
				std::string emissive_map_filename = "";
				if (color_map_filename.find("Color.png") != std::string::npos)
				{
					emissive_map_filename = color_map_filename.erase(color_map_filename.find("Color.png")) + "E.png";
				}
				std::filesystem::path fbm_filename(fbx_filename);
				fbm_filename.remove_filename();
				std::string s = fbm_filename.string() + emissive_map_filename;
				if (std::filesystem::exists(s.c_str()))
				{
					material.texture_filenames[5] = emissive_map_filename;
				}
				else
				{
					material.texture_filenames[5] = "";
				}
			}
			// Dissolve map
			{
				std::string color_map_filename = material.texture_filenames[0];
				std::string dissolve_map_filename = "";
				if (color_map_filename.find("Color.png") != std::string::npos)
				{
					dissolve_map_filename = color_map_filename.erase(color_map_filename.find("Color.png")) + "Dissolve.png";
				}
				std::filesystem::path fbm_filename(fbx_filename);
				fbm_filename.remove_filename();
				std::string s = fbm_filename.string() + dissolve_map_filename;
				if (std::filesystem::exists(s.c_str()))
				{
					material.texture_filenames[6] = dissolve_map_filename;
				}
				else
				{
					material.texture_filenames[6] = "";
				}
			}
			// Glow map
			{
				std::string color_map_filename = material.texture_filenames[0];
				std::string glow_map_filename = "";
				if (color_map_filename.find("Color.png") != std::string::npos)
				{
					glow_map_filename = color_map_filename.erase(color_map_filename.find("Color.png")) + "Glow.png";
				}
				std::filesystem::path fbm_filename(fbx_filename);
				fbm_filename.remove_filename();
				std::string s = fbm_filename.string() + glow_map_filename;
				if (std::filesystem::exists(s.c_str()))
				{
					material.texture_filenames[7] = glow_map_filename;
				}
				else
				{
					material.texture_filenames[7] = "";
				}
			}

			materials.emplace(material.unique_id, std::move(material));
		}
	}

	materials.emplace();
}


//-----------------------------------------------//
//メッシュの抽出
//----------------------------------------------//
void ModelResource::fetch_meshes(FbxScene* fbx_scene, std::vector<mesh>& meshes, float sampling_rate)
{
	for (const scene::node& node : scene_view.nodes)
	{
		if (node.attribute != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}

		FbxNode* fbx_node{ FindFbxNodeByUniqueId(fbx_scene, node.unique_id) };
		//FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
		FbxMesh* fbx_mesh{ fbx_node->GetMesh() };

		mesh& mesh{ meshes.emplace_back() };
		mesh.unique_id = fbx_mesh->GetNode()->GetUniqueID();
		mesh.name = fbx_mesh->GetNode()->GetName();
		mesh.node_index = scene_view.indexof(mesh.unique_id);
		mesh.default_global_transform = to_xmfloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());

		//ボーンの影響度抽出
		std::vector<bone_influences_per_control_point> bone_influences;
		fetch_bone_influences(fbx_mesh, bone_influences);

		//スケルトン抽出
		fetch_skeleton(fbx_mesh, mesh.bind_pose);


		//バウンディングボックス
		for (const vertex& v : mesh.vertices)
		{
			mesh.bounding_box[0].x = std::min<float>(mesh.bounding_box[0].x, v.position.x);
			mesh.bounding_box[0].y = std::min<float>(mesh.bounding_box[0].y, v.position.y);
			mesh.bounding_box[0].z = std::min<float>(mesh.bounding_box[0].z, v.position.z);
			mesh.bounding_box[1].x = std::max<float>(mesh.bounding_box[1].x, v.position.x);
			mesh.bounding_box[1].y = std::max<float>(mesh.bounding_box[1].y, v.position.y);
			mesh.bounding_box[1].z = std::max<float>(mesh.bounding_box[1].z, v.position.z);
		}


		std::vector<mesh::subset>& subsets{ mesh.subsets };
		const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };
		subsets.resize(material_count > 0 ? material_count : 1);
		for (int material_index = 0; material_index < material_count; ++material_index)
		{
			const FbxSurfaceMaterial* fbx_material{ fbx_mesh->GetNode()->GetMaterial(material_index) };
			subsets.at(material_index).material_name = fbx_material->GetName();
			subsets.at(material_index).material_unique_id = fbx_material->GetUniqueID();
		}
		if (material_count > 0)
		{
			const int polygon_count{ fbx_mesh->GetPolygonCount() };
			for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
			{
				const int material_index
				{ fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) };
				subsets.at(material_index).index_count += 3;
			}
			uint32_t offset{ 0 };
			for (mesh::subset& subset : subsets)
			{
				subset.start_index_location = offset;
				offset += subset.index_count;
				// 次の手順でカウンターとして使用され、ゼロにリセットされる
				subset.index_count = 0;
			}

		}

		const int polygon_count{ fbx_mesh->GetPolygonCount() };
		mesh.vertices.resize(polygon_count * 3LL);
		mesh.indices.resize(polygon_count * 3LL);

		FbxStringList uv_names;
		fbx_mesh->GetUVSetNames(uv_names);
		const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };
		for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
		{
			const int material_index{ material_count > 0 ? fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
			mesh::subset& subset{ subsets.at(material_index) };
			const uint32_t offset{ subset.start_index_location + subset.index_count };

			for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
			{
				const int vertex_index{ polygon_index * 3 + position_in_polygon };

				vertex vertex;
				const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index, position_in_polygon) };
				vertex.position.x = static_cast<float>(control_points[polygon_vertex][0]);
				vertex.position.y = static_cast<float>(control_points[polygon_vertex][1]);
				vertex.position.z = static_cast<float>(control_points[polygon_vertex][2]);

				//UNIT22
				const bone_influences_per_control_point& influences_per_control_point
				{ bone_influences.at(polygon_vertex) };
				for (size_t influence_index = 0; influence_index < influences_per_control_point.size();
					++influence_index)
				{
					if (influence_index < MAX_BONE_INFLUENCES)
					{
						vertex.bone_weights[influence_index] =
							influences_per_control_point.at(influence_index).bone_weight; //頂点にウェイトを設定
						vertex.bone_indices[influence_index] =
							influences_per_control_point.at(influence_index).bone_index;//頂点にボーンインデックスを設定
					}
				}


				if (fbx_mesh->GetElementNormalCount() > 0)
				{
					FbxVector4 normal;
					fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}
				if (fbx_mesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmapped_uv;
					fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon,
						uv_names[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}
				//UNIT29
				if (fbx_mesh->GenerateTangentsData(0, false))
				{
					const FbxGeometryElementTangent* tangent = fbx_mesh->GetElementTangent(0);
					vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[0]);
					vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[1]);
					vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[2]);
					vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[3]);
				}

				mesh.vertices.at(vertex_index) = std::move(vertex);
				mesh.indices.at(static_cast<size_t>(offset) + position_in_polygon) = vertex_index;
				subset.index_count++;
			}
		}
	}
}

//-----------------------------------------------//
 /// バインドポーズ(初期姿勢)の情報の取り出し 
//----------------------------------------------//
void ModelResource::fetch_skeleton(FbxMesh* fbx_mesh, skeleton& bind_pose)
{
	//メッシュにあるスキンの数を取得
	const int deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
	// メッシュにあるすべてのスキンの情報をチェック
	for (int deformer_index = 0; deformer_index < deformer_count; ++deformer_index)
	{
		// 現在のスキンを取得
		FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(deformer_index, FbxDeformer::eSkin));
		// スキンにあるクラスターの数を取得
		const int cluster_count = skin->GetClusterCount();
		// メッシュのボーンの数 = メッシュのクラスターの数
		bind_pose.bones.resize(cluster_count);
		// スキンにあるすべてのクラスターの情報をチェック
		for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
		{
			FbxCluster* cluster = skin->GetCluster(cluster_index);

			skeleton::bone& bone{ bind_pose.bones.at(cluster_index) };
			bone.name = cluster->GetLink()->GetName();
			bone.unique_id = cluster->GetLink()->GetUniqueID();
			bone.parent_index = bind_pose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());
			bone.node_index = scene_view.indexof(bone.unique_id);

			//モデル(メッシュ)のローカル空間からシーンのグローバル空間に変換するために使われる
			FbxAMatrix reference_global_init_position;
			cluster->GetTransformMatrix(reference_global_init_position);

			//ボーンのローカル空間からシーンのグローバル空間への変換に使用
			FbxAMatrix cluster_global_init_position;
			cluster->GetTransformLinkMatrix(cluster_global_init_position);

			// 行列はColumn Major スキームを使用して定義、
			//FbxAMatrixが変換（平行移動、回転、拡大縮小）を表す場合、行列の最後の行は変換の平行移動部分を表す
			//メッシュ空間からボーン空間に位置を変換する'bone.offset_transform'行列を作成
			bone.offset_transform
				= to_xmfloat4x4(cluster_global_init_position.Inverse() * reference_global_init_position);
		}
	}
}

//-----------------------------------------------//
//アニメーション情報の抽出
//----------------------------------------------//
void ModelResource::fetch_animations(FbxScene* fbx_scene, std::vector<animation>& animation_clips, float sampling_rate/*値が0の場合、アニメーションデータはデフォルトのフレームレートでサンプリングされる.*/)
{

	//アニメーション名一覧を格納する配列の宣言
	FbxArray<FbxString*> animation_stack_names;
	//シーンからアニメーション一覧を取得＆格納
	fbx_scene->FillAnimStackNameArray(animation_stack_names);
	const int animation_stack_count{ animation_stack_names.GetCount() };
	for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
	{
		//アニメーションを取得
		animation& animation_clip{ animation_clips.emplace_back() };
		animation_clip.name = animation_stack_names[animation_stack_index]->Buffer();
		//取得したアニメーションをアニメーションスタックとして設定
		FbxAnimStack* animation_stack{ fbx_scene->FindMember<FbxAnimStack>(animation_clip.name.c_str()) };
		//選んだアニメーションの設定
		fbx_scene->SetCurrentAnimationStack(animation_stack);

		//時間関係の計算
		const FbxTime::EMode time_mode{ fbx_scene->GetGlobalSettings().GetTimeMode() }; //時間のカウントモードの設定
		FbxTime one_second;
		one_second.SetTime(0, 0, 1, 0, 0, time_mode);
		animation_clip.sampling_rate = sampling_rate > 0 ?
			sampling_rate : static_cast<float>(one_second.GetFrameRate(time_mode));
		//サンプリングレートからサンプリングの間隔時間を設定
		const FbxTime sampling_interval
		{ static_cast<FbxLongLong>(one_second.Get() / animation_clip.sampling_rate) };

		// アニメーションの開始と終了を取得
		const FbxTakeInfo* take_info{ fbx_scene->GetTakeInfo(animation_clip.name.c_str()) };
		const FbxTime start_time{ take_info->mLocalTimeSpan.GetStart() };
		const FbxTime stop_time{ take_info->mLocalTimeSpan.GetStop() };
		for (FbxTime time = start_time; time < stop_time; time += sampling_interval)
		{
			// サンプリング間隔ごとのキーフレームを取得
			animation::keyframe& keyframe{ animation_clip.sequence.emplace_back() };

			const size_t node_count{ scene_view.nodes.size() };
			keyframe.nodes.resize(node_count);
			for (size_t node_index = 0; node_index < node_count; ++node_index)
			{
				FbxNode* fbx_node{ FindFbxNodeByUniqueId(fbx_scene, scene_view.nodes.at(node_index).unique_id) }; //UEのアセットに対応
			//	FbxNode* fbx_node{ fbx_scene->FindNodeByName(scene_view.nodes.at(node_index).name.c_str()) };
				if (fbx_node)
				{
					animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
					/// アニメーション時間からアニメーション行列を取得
					node.global_transform = to_xmfloat4x4(fbx_node->EvaluateGlobalTransform(time));

					//UNIT27
					const FbxAMatrix& local_transform{ fbx_node->EvaluateLocalTransform(time) };
					node.scaling = to_xmfloat3(local_transform.GetS()); //スケール取り出し
					node.rotation = to_xmfloat4(local_transform.GetQ());//rotation取り出し（クォータニオン）
					node.translation = to_xmfloat3(local_transform.GetT());//transform取り出し

				}
			}
		}
	}
	for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
	{
		delete animation_stack_names[animation_stack_index];
	}

}

//-----------------------------------------------//
//アニメーションの登録
//----------------------------------------------//
bool ModelResource::append_animations(const char* animation_filename, float sampling_rate)
{
	FbxManager* fbx_manager{ FbxManager::Create() };
	FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };

	FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };
	bool import_status{ false };
	import_status = fbx_importer->Initialize(animation_filename);
	_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
	import_status = fbx_importer->Import(fbx_scene);
	_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

	fetch_animations(fbx_scene, animation_clips, sampling_rate);

	fbx_manager->Destroy();

	return true;
}



FbxNode* ModelResource::FindFbxNodeByUniqueId(FbxNode* fbx_node, uint64_t id)
{
	if (fbx_node->GetUniqueID() == id)
	{
		return fbx_node;
	}

	for (int child_index = 0; child_index < fbx_node->GetChildCount(); child_index++)
	{
		FbxNode* result = FindFbxNodeByUniqueId(fbx_node->GetChild(child_index), id);
		if (result != nullptr)
		{
			return result;
		}
	}
	return nullptr;
}

FbxNode* ModelResource::FindFbxNodeByUniqueId(FbxScene* fbx_scene, uint64_t id)
{
	return FindFbxNodeByUniqueId(fbx_scene->GetRootNode(), id);
}
