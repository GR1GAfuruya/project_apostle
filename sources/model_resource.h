#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>
#include<string>
#include <fbxsdk.h>
#include <unordered_map>


#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>

#include "constant.h"


//シリアライズ
namespace DirectX
{
	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT2& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT3& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4X4& m)
	{
		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
			cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
			cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
			cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
			cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
}


struct skeleton
{
	//メッシュのボーン情報
	struct bone
	{
		uint64_t unique_id{ 0 };
		std::string name;
		// parent_indexは、自分自身を含む配列の中での親ボーンの位置を参照するインデックス
		int64_t parent_index{ -1 }; // -1 : the bone is orphan
		// node_index は、シーンのノード配列を参照するインデックス
		int64_t node_index{ 0 };

		// offset_transformは、モデル(メッシュ)空間からボーン(ノード)シーンへの変換に使用される
		DirectX::XMFLOAT4X4 offset_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

		// true...親無しのボーン
		bool is_orphan() const { return parent_index < 0; };

		//シリアライズ
		template<class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, parent_index, node_index, offset_transform);
		}


	};
	std::vector<bone> bones;
	int64_t indexof(uint64_t unique_id) const
	{
		int64_t index{ 0 };
		for (const bone& bone : bones)
		{
			if (bone.unique_id == unique_id)
			{
				return index;
			}
			++index;
		}
		return -1;
	}

	//シリアライズ
	template<class T>
	void serialize(T& archive)
	{
		archive(bones);
	}

};
struct animation
{

	struct keyframe
	{
		struct node
		{
			// ノードのローカル空間からシーンのグローバル空間への変換
			DirectX::XMFLOAT4X4 global_transform{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1 };
			DirectX::XMFLOAT4X4 local_transform{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1 };
			DirectX::XMFLOAT3 scaling{ 1, 1, 1 };
			DirectX::XMFLOAT4 rotation{ 0, 0, 0, 1 }; //クォータニオン
			DirectX::XMFLOAT3 translation{ 0, 0, 0 };

			template<class T >
			void serialize(T& archive)
			{
				archive(global_transform, scaling, rotation, translation);
			}

		};
		std::vector<node> nodes;
		template<class T>
		void serialize(T& archive)
		{
			archive(nodes);
		}

	};


	std::string name;
	float sampling_rate{ 0 };

	std::vector<keyframe> sequence;

	//シリアライズ
	template<class T>
	void serialize(T& archive)
	{
		archive(name, sampling_rate, sequence);
	}

};


//シーン構造体
struct scene
{
	struct node
	{
		uint64_t unique_id{ 0 };
		std::string name;
		FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
		int64_t parent_index{ -1 };

		//シリアライズ
		template<class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, attribute, parent_index);
		}

	};
	std::vector<node> nodes;
	int64_t indexof(uint64_t unique_id) const
	{
		int64_t index{ 0 };
		for (const node& node : nodes)
		{
			if (node.unique_id == unique_id)
			{
				return index;
			}
			++index;
		}
		return -1;
	}

	//シリアライズ
	template<class T>
	void serialize(T& archive)
	{
		archive(nodes);
	}


};


class ModelResource
{
public:
	ModelResource(ID3D11Device* device, const char* fbx_filename, float sampling_rate, bool triangulate = false);
	virtual ~ModelResource() = default;
	static const int MAX_BONE_INFLUENCES{ 4 };
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 tangent;
		DirectX::XMFLOAT2 texcoord;
		//ボーンウェイト
		float bone_weights[MAX_BONE_INFLUENCES]{ 1, 0, 0, 0 };
		uint32_t bone_indices[MAX_BONE_INFLUENCES]{};

		//シリアライズ
		template<class T>
		void serialize(T& archive)
		{
			archive(position, normal, tangent, texcoord, bone_weights, bone_indices);
		}

	};



	struct mesh
	{

		struct subset
		{
			uint64_t material_unique_id{ 0 };
			std::string material_name;

			uint32_t start_index_location{ 0 };
			uint32_t index_count{ 0 };


			//シリアライズ
			template<class T>
			void serialize(T& archive)
			{
				archive(material_unique_id, material_name, start_index_location, index_count);
			}

		};
		std::vector<subset> subsets;


		uint64_t unique_id{ 0 };
		std::string name;

		int64_t node_index{ 0 }; //シーンのノード配列を参照するインデックス

		std::vector<vertex> vertices;
		std::vector<uint32_t> indices;

		DirectX::XMFLOAT4X4 default_global_transform{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 };

		skeleton bind_pose;

		DirectX::XMFLOAT3 bounding_box[2]
		{
			{ +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX },
			{ -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX }
		};




		//シリアライズ
		template<class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, node_index, subsets, default_global_transform,
				bind_pose, bounding_box, vertices, indices);
		}

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

	};
	std::vector<mesh> meshes;

	struct material
	{
		uint64_t unique_id{ 0 };
		std::string name;

		DirectX::XMFLOAT4 ambient{ 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 diffuse{ 0.8f, 0.8f, 0.8f, 1.0f };
		DirectX::XMFLOAT4 specular{ 1.0f, 1.0f, 1.0f, 1.0f };
		static constexpr int TEXTURE_COUNT = 8;
		std::string texture_filenames[TEXTURE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[TEXTURE_COUNT];

		//シリアライズ
		template<class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, ambient, diffuse, specular, texture_filenames);
		}

	};
	std::unordered_map<uint64_t, material> materials;


	
private:
	scene scene_view;
	std::vector<animation> animation_clips;

	void create_com_objects(ID3D11Device* device, const char* fbx_filename);
	//ゲッター
public:
	std::vector<mesh>& get_meshes() { return meshes; }
	scene& get_scene_view() { return scene_view; }
	const std::vector<animation>& get_animation_clips() { return animation_clips; }
	//モデル情報抽出
protected:
	void fetch_meshes(FbxScene* fbx_scene, std::vector<mesh>& meshes, float sampling_rate); //メッシュの抽出

	void fetch_materials(const char* fbx_filename, FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials); //マテリアルの抽出

	void fetch_skeleton(FbxMesh* fbx_mesh, skeleton& bind_pose);

	//アニメーション抽出
	void fetch_animations(FbxScene* fbx_scene, std::vector<animation>& animation_clips,
		float sampling_rate);

	//アニメーションの登録
	bool append_animations(const char* animation_filename, float sampling_rate);

	FbxNode* FindFbxNodeByUniqueId(FbxNode* fbx_node, uint64_t id);

	FbxNode* FindFbxNodeByUniqueId(FbxScene* fbx_scene, uint64_t id);


};
