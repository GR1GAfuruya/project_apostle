#include "skeletal_mesh.h"

using namespace DirectX;



#include "resource_manager.h"
#include "user.h"


SkeletalMesh::SkeletalMesh(ID3D11Device* device, const char* fbx_filename, float sampling_rate, bool triangulate)
{
	model_resource = ResourceManager::instance().load_model_resource(device, fbx_filename, triangulate, sampling_rate);
}

//-----------------------------------------------//
//�A�j���[�V�����̎p���A�b�v�f�[�g
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
//�A�j���[�V�����̃A�b�v�f�[�g
//----------------------------------------------//
void SkeletalMesh::update_animation(float elapsed_time)
{
	//�A�j���[�V�������I�����Ă����ꍇ�X�V���Ȃ�
	if (is_end_animation()) return;

	// �A�j���[�V�������A�j���[�V�����̃t���[���Z�o
	anime_param.animation = model_resource.get()->get_animation_clips().at(anime_param.current_index);
	anime_param.frame_index = static_cast<int>(anime_param.current_time * anime_param.animation.sampling_rate);

	//�u�����h���̌v�Z
	float blendRate = 1.0f;
	if (anime_param.blend_time < anime_param.blend_seconds) {
		anime_param.blend_time += elapsed_time;

		if (anime_param.blend_time >= anime_param.blend_seconds) {
			anime_param.blend_time = anime_param.blend_seconds;
		}
		//�u�����h���@���@�u�����h���Ă���̎��ԁ@/�@�u�����h�����܂ł̎���
		blendRate = anime_param.blend_time / anime_param.blend_seconds;
		blendRate *= blendRate;
	}

	if (blendRate < 1.0f)
	{
		//[0]:�J�ڑO�̃A�j���[�V�����̃t���[�����@[1]:�J�ڌ�̃A�j���[�V�����̃t���[����
		const animation::keyframe* keyframes[2] =
		{ &model_resource.get()->get_animation_clips().at(anime_param.old_anime_index).sequence.at(anime_param.frame_index),
			& model_resource.get()->get_animation_clips().at(anime_param.current_index).sequence.at(anime_param.frame_index) };
		//2�̃L�[�t���[���Ԃ̕⊮�v�Z
		blend_animations(keyframes, blendRate, anime_param.animation_keyframe);
		update_animation_transform(anime_param.animation_keyframe);
	}
	else
	{
		//�A�j���[�V��������������Ƃ��̏���
		if (anime_param.frame_index > anime_param.animation.sequence.size() - 1)
		{
			//���[�v�Đ���ON�Ȃ�A�j���[�V�����̍ŏ��ɖ߂�
			if (anime_param.loop_flag)
			{
				anime_param.frame_index = 0;
				anime_param.current_time = 0;

			}
			//���[�v�Đ���OFF�Ȃ炻�̂܂ܒ�~
			else
			{
				anime_param.end_flag = true;
				anime_param.frame_index = static_cast<int>(anime_param.animation.sequence.size() - 1);
			}
		}
		//�Đ���
		else
		{
			//�A�j���[�V�������I�[�܂ōs���Ă��Ȃ���Ύ��Ԃ�i�߂�
			if (!is_end_animation())
			{
				anime_param.current_time += anime_param.playback_speed * elapsed_time;
			}
		}
		anime_param.animation_keyframe = anime_param.animation.sequence.at(anime_param.frame_index);
	}
}

//-----------------------------------------------//
//�A�j���[�V�����̍Đ�
//----------------------------------------------//
void SkeletalMesh::play_animation(int index, bool loop, float blend_seconds)
{
	anime_param.old_anime_index = anime_param.current_index;//�؂�ւ��O�ɍs���Ă����A�j���[�V�������L�^
	anime_param.current_index = index;//�ǂ̃A�j���[�V�������Đ����邩�ݒ�
	anime_param.frame_index = 0;
	anime_param.current_time = 0.0f;//�Đ����Ă���̎���
	anime_param.playback_speed = 1.0f;//�A�j���[�V�����Đ����x��ݒ�
	anime_param.loop_flag = loop;//���[�v�Đ����邩�ǂ�����ݒ�
	anime_param.end_flag = false;
	anime_param.blend_time = 0.0f;
	anime_param.blend_seconds = blend_seconds;//�A�j���[�V������ԗ���ݒ�
}


//-----------------------------------------------//
//�A�j���[�V�����Ԃ̕��
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
		XMStoreFloat3(&keyframe.nodes.at(node_index).scaling, XMVectorLerp(S[0], S[1], blend_rate)); //�O��̑傫������`�⊮

		XMVECTOR R[2]{
			XMLoadFloat4(&keyframes[0]->nodes.at(node_index).rotation),
			XMLoadFloat4(&keyframes[1]->nodes.at(node_index).rotation) };
		XMStoreFloat4(&keyframe.nodes.at(node_index).rotation, XMQuaternionSlerp(R[0], R[1], blend_rate));//�O��̉�]����`�⊮

		XMVECTOR T[2]{ XMLoadFloat3(&keyframes[0]->nodes.at(node_index).translation),
			XMLoadFloat3(&keyframes[1]->nodes.at(node_index).translation) };
		XMStoreFloat3(&keyframe.nodes.at(node_index).translation, XMVectorLerp(T[0], T[1], blend_rate));//�O��̈ʒu����`�⊮
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
	_ASSERT_EXPR(false, "�w�肳�ꂽ���O�̃{�[��������܂���");
	return dummy;
}

DirectX::XMFLOAT3 SkeletalMesh::root_defference_pos_next_frame(const DirectX::XMFLOAT4X4& world, const skeleton::bone& bone)
{
	//���̃t���[���̃A�j���[�V�����Ƃ̈ʒu����
	DirectX::XMFLOAT3 defference_pos;
	//�A�j���[�V������1�����ĂȂ����`�F�b�N
	if (anime_param.frame_index + 1 < static_cast<int>(anime_param.animation.sequence.size() - 1))
	{
		//���̃t���[���̃A�j���[�V�����̃L�[�t���[��
		const animation::keyframe next_animation_keyframe = anime_param.animation.sequence.at(anime_param.frame_index + 1);

			const animation::keyframe::node& current_bone_node{ (&anime_param.animation_keyframe)->nodes.at(bone.node_index) };
			const animation::keyframe::node& next_bone_node{ (&next_animation_keyframe)->nodes.at(bone.node_index) };
			DirectX::XMFLOAT4X4 current_bone_transform;
			DirectX::XMFLOAT4X4 next_bone_transform;
			XMStoreFloat4x4(&current_bone_transform, XMLoadFloat4x4(&current_bone_node.global_transform) );
			XMStoreFloat4x4(&next_bone_transform, XMLoadFloat4x4(&next_bone_node.global_transform) );

			DirectX::XMFLOAT3 current_bone_pos = { current_bone_transform._41,current_bone_transform._42,current_bone_transform._43 };
			DirectX::XMFLOAT3 next_bone_pos = { next_bone_transform._41,next_bone_transform._42,next_bone_transform._43 };
			defference_pos = Math::calc_vector_AtoB(current_bone_pos, next_bone_pos);
			return defference_pos;
	}
	return { 0,0,0 };
}

float SkeletalMesh::root_defference_length_next_frame(const skeleton::bone& bone)
{
	//���̃t���[���̃A�j���[�V�����Ƃ̈ʒu����
	float defference_length;
	//�A�j���[�V������1�����ĂȂ����`�F�b�N
	if (anime_param.frame_index > 0)
	{
		//���̃t���[�����A�j���[�V�����̍ő�t���[���������Ă��Ȃ����`�F�b�N
		if (anime_param.frame_index + 1 < static_cast<int>(anime_param.animation.sequence.size() - 1))
		{
			//���̃t���[���̃A�j���[�V�����̃L�[�t���[��
			const animation::keyframe next_animation_keyframe = anime_param.animation.sequence.at(anime_param.frame_index + 1);

			const animation::keyframe::node& current_bone_node{ (&anime_param.animation_keyframe)->nodes.at(bone.node_index) };
			const animation::keyframe::node& next_bone_node{ (&next_animation_keyframe)->nodes.at(bone.node_index) };
			DirectX::XMFLOAT4X4 current_bone_transform;
			DirectX::XMFLOAT4X4 next_bone_transform;
			XMStoreFloat4x4(&current_bone_transform, XMLoadFloat4x4(&current_bone_node.global_transform));
			XMStoreFloat4x4(&next_bone_transform, XMLoadFloat4x4(&next_bone_node.global_transform));

			DirectX::XMFLOAT3 current_bone_pos = { current_bone_transform._41,current_bone_transform._42,current_bone_transform._43 };
			DirectX::XMFLOAT3 next_bone_pos = { next_bone_transform._41,next_bone_transform._42,next_bone_transform._43 };
			DirectX::XMFLOAT2 current_bone_pos2 = { current_bone_pos.x,current_bone_pos.z };
			DirectX::XMFLOAT2 next_bone_pos2 = { next_bone_pos.x,next_bone_pos.z };
			//XZ�̂�
			defference_length = Math::calc_vector_AtoB_length(current_bone_pos2, next_bone_pos2);

			return defference_length;
		}
	}
	return 0.0f;
}

void SkeletalMesh::fech_by_bone(const DirectX::XMFLOAT4X4& world, const skeleton::bone& bone, DirectX::XMFLOAT3& pos, DirectX::XMFLOAT4X4* mat)
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

		//�����ɂ��p�����v������Ă���ꍇ
		if (mat)
		{
			DirectX::XMStoreFloat4x4(mat, R);		
		}
		
	}
}

void SkeletalMesh::fech_bone_world_matrix(const DirectX::XMFLOAT4X4& world, const skeleton::bone& bone, DirectX::XMFLOAT4X4* mat)
{
	if (&anime_param.animation_keyframe && (&anime_param.animation_keyframe)->nodes.size() > 0)
	{
		const animation::keyframe::node& bone_node{ (&anime_param.animation_keyframe)->nodes.at(bone.node_index) };
		DirectX::XMFLOAT4X4 w;
		XMStoreFloat4x4(&w, XMLoadFloat4x4(&bone_node.global_transform) * XMLoadFloat4x4(&world));

		DirectX::XMFLOAT3 scale = { Math::Length({w._11,w._12,w._13}),  Math::Length({w._21,w._22,w._23}),  Math::Length({w._31,w._32,w._33}) };

		*mat = w;

	}
}
