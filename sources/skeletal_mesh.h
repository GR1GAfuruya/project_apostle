#pragma once
#include "model_resource.h"

class SkeletalMesh
{
public:
	SkeletalMesh(ID3D11Device* device, const char* fbx_filename, float sampling_rate = 60.0f, bool triangulate = false);
	virtual ~SkeletalMesh() = default;

	ModelResource* get_resource() { return model_resource.get(); }

private:
	static const int MAX_BONES{ 256 };

	
public:
	struct AnimeParam
	{
		int old_anime_index = 0;//直前に再生していたアニメーション
		int current_index = 0;//再生するアニメーションの種類
		int frame_index = 0;
		float playback_speed = 0.0f;//そのアニメーションの再生速度
		float current_time = 0.0f;//そのアニメーションが何秒であるか
		bool loop_flag = false;//ループ再生するアニメーションか
		bool end_flag = false;////アニメーションの終わりか
		float blend_time = 0.0f;//次のアニメーションへの補完時間
		float blend_seconds = 0.0f;
		animation animation = {};
		animation::keyframe animation_keyframe = {};
	};
	AnimeParam anime_param;
	//アニメーション更新
	void update_animation(float elapsed_time);
	//アニメーション再生
	void play_animation( int index, bool loop, float blend_seconds = 0.2f);

	//アニメーションが終了したか
	bool is_end_animation() const { return anime_param.end_flag; }

	//void render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world);


	void update_animation_transform(animation::keyframe& keyframe);


	//アニメーション間の補完計算
	void blend_animations(const animation::keyframe* keyframes[2], float factor, animation::keyframe& keyframe);

	std::shared_ptr<ModelResource> model_resource;
	//void fech_by_bone(const DirectX::XMFLOAT4X4& world, const skeleton::bone& bone, DirectX::XMFLOAT3& pos, DirectX::XMFLOAT4* ori = nullptr);
	void fech_by_bone(const DirectX::XMFLOAT4X4& world, const skeleton::bone& bone, DirectX::XMFLOAT3& pos, DirectX::XMFLOAT4X4* mat = nullptr);

	const skeleton::bone& get_bone_by_name(std::string name);

};


