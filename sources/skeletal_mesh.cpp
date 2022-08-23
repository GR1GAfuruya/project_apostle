#include "skeletal_mesh.h"

using namespace DirectX;



#include "resource_manager.h"
#include "user.h"


SkeletalMesh::SkeletalMesh(ID3D11Device* device, const char* fbx_filename, float sampling_rate, bool triangulate)
{
	model_resource = ResourceManager::instance().load_model_resource(device, fbx_filename, triangulate, sampling_rate);
}

//-----------------------------------------------//
//アニメーションの姿勢アップデート
//----------------------------------------------//
void SkeletalMesh::update_animation_transform(animation::keyframe& keyframe)
{
	size_t node_count{ keyframe.nodes.size() };
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
		XMMATRIX S{ XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z) };
		XMMATRIX R{ XMMatrixRotationQuaternion(XMLoadFloat4(&node.rotation)) };
		XMMATRIX T{ XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z) };

		int64_t parent_index{ model_resource.get()->get_scene_view().nodes.at(node_index).parent_index };
		XMMATRIX P{ parent_index < 0 ? XMMatrixIdentity() :
			XMLoadFloat4x4(&keyframe.nodes.at(parent_index).global_transform) };

		XMStoreFloat4x4(&node.local_transform, S * R * T );
		XMStoreFloat4x4(&node.global_transform, S * R * T * P);

	}
}

//-----------------------------------------------//
//アニメーションのアップデート
//----------------------------------------------//
void SkeletalMesh::update_animation(float elapsed_time)
{
	//アニメーションが終了していた場合更新しない
	if (is_end_animation()) return;

	// アニメーション＆アニメーションのフレーム算出
	anime_param.animation = model_resource.get()->get_animation_clips().at(anime_param.current_index);
	anime_param.frame_index = static_cast<int>(anime_param.current_time * anime_param.animation.sampling_rate);

	//ブレンド率の計算
	float blendRate = 1.0f;
	if (anime_param.blend_time < anime_param.blend_seconds) {
		anime_param.blend_time += elapsed_time;

		if (anime_param.blend_time >= anime_param.blend_seconds) {
			anime_param.blend_time = anime_param.blend_seconds;
		}
		//ブレンド率　＝　ブレンドしてからの時間　/　ブレンド完了までの時間
		blendRate = anime_param.blend_time / anime_param.blend_seconds;
		blendRate *= blendRate;
	}

	if (blendRate < 1.0f)
	{
		//[0]:遷移前のアニメーションのフレーム数　[1]:遷移後のアニメーションのフレーム数
		const animation::keyframe* keyframes[2] =
		{ &model_resource.get()->get_animation_clips().at(anime_param.old_anime_index).sequence.at(anime_param.frame_index),
			& model_resource.get()->get_animation_clips().at(anime_param.current_index).sequence.at(anime_param.frame_index) };
		//2つのキーフレーム間の補完計算
		blend_animations(keyframes, blendRate, anime_param.animation_keyframe);
		update_animation_transform(anime_param.animation_keyframe);
	}
	else
	{
		//アニメーションが一周したときの処理
		if (anime_param.frame_index > anime_param.animation.sequence.size() - 1)
		{
			//ループ再生がONならアニメーションの最初に戻す
			if (anime_param.loop_flag)
			{
				anime_param.frame_index = 0;
				anime_param.current_time = 0;

			}
			//ループ再生がOFFならそのまま停止
			else
			{
				anime_param.end_flag = true;
				anime_param.frame_index = static_cast<int>(anime_param.animation.sequence.size() - 1);
			}
		}
		//再生中
		else
		{
			//アニメーションが終端まで行っていなければ時間を進める
			if (!is_end_animation())
			{
				anime_param.current_time += elapsed_time;
			}
		}
		anime_param.animation_keyframe = anime_param.animation.sequence.at(anime_param.frame_index);
	}
}

//-----------------------------------------------//
//アニメーションの再生
//----------------------------------------------//
void SkeletalMesh::play_animation(int index, bool loop, float blend_seconds)
{
	anime_param.old_anime_index = anime_param.current_index;//切り替わる前に行っていたアニメーションを記録
	anime_param.current_index = index;//どのアニメーションを再生するか設定
	anime_param.frame_index = 0;
	anime_param.current_time = 0.0f;//再生してからの時間
	anime_param.loop_flag = loop;//ループ再生するかどうかを設定
	anime_param.end_flag = false;
	anime_param.blend_time = 0.0f;
	anime_param.blend_seconds = blend_seconds;//アニメーション補間率を設定
}


//-----------------------------------------------//
//アニメーション間の補間
//----------------------------------------------//
void SkeletalMesh::blend_animations(const animation::keyframe* keyframes[2], float blend_rate, animation::keyframe& keyframe)
{
	size_t node_count{ keyframes[0]->nodes.size() };
	keyframe.nodes.resize(node_count);
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		XMVECTOR S[2]{
			XMLoadFloat3(&keyframes[0]->nodes.at(node_index).scaling),
			XMLoadFloat3(&keyframes[1]->nodes.at(node_index).scaling) };
		XMStoreFloat3(&keyframe.nodes.at(node_index).scaling, XMVectorLerp(S[0], S[1], blend_rate)); //前後の大きさを線形補完

		XMVECTOR R[2]{
			XMLoadFloat4(&keyframes[0]->nodes.at(node_index).rotation),
			XMLoadFloat4(&keyframes[1]->nodes.at(node_index).rotation) };
		XMStoreFloat4(&keyframe.nodes.at(node_index).rotation, XMQuaternionSlerp(R[0], R[1], blend_rate));//前後の回転を線形補完

		XMVECTOR T[2]{ XMLoadFloat3(&keyframes[0]->nodes.at(node_index).translation),
			XMLoadFloat3(&keyframes[1]->nodes.at(node_index).translation) };
		XMStoreFloat3(&keyframe.nodes.at(node_index).translation, XMVectorLerp(T[0], T[1], blend_rate));//前後の位置を線形補完
	}

}

const skeleton::bone& SkeletalMesh::get_bone_by_name(std::string name)
{
	skeleton::bone dummy = {};
	for (const ModelResource::mesh& mesh : model_resource->get_meshes())
	{
		const size_t bone_count{ mesh.bind_pose.bones.size() };
		_ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");
		for (int bone_index = 0; bone_index < bone_count; ++bone_index)
		{
			const skeleton::bone& bone = mesh.bind_pose.bones.at(bone_index);
			if (bone.name == name)
			{
				return bone;
			}
		}
	}
	_ASSERT_EXPR(false, "指定された名前のボーンがありません");
	return dummy;
}

void SkeletalMesh::fech_by_bone(const DirectX::XMFLOAT4X4& world, const skeleton::bone& bone, DirectX::XMFLOAT3& pos, DirectX::XMFLOAT3& up)
{
	if (&anime_param.animation_keyframe && (&anime_param.animation_keyframe)->nodes.size() > 0)
	{
		const animation::keyframe::node& bone_node{ (&anime_param.animation_keyframe)->nodes.at(bone.node_index) };
		DirectX::XMFLOAT4X4 w;
		XMStoreFloat4x4(&w, XMLoadFloat4x4(&bone_node.global_transform) * XMLoadFloat4x4(&world));

		pos = { w._41,w._42,w._43 };
		DirectX::XMFLOAT3 scale = { Math::Length({w._11,w._12,w._13}),  Math::Length({w._21,w._22,w._23}),  Math::Length({w._31,w._32,w._33}) };

		DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) };
		DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z) };
		DirectX::XMMATRIX R = DirectX::XMLoadFloat4x4(&w) * DirectX::XMMatrixInverse(nullptr, S) * DirectX::XMMatrixInverse(nullptr, T);

		DirectX::XMFLOAT4X4 r = {};
		DirectX::XMStoreFloat4x4(&r, R);
		DirectX::XMVECTOR right_vec = { r._11, r._12, r._13 };
		XMStoreFloat3(&up, right_vec);
	}
}

