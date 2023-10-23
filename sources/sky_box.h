#pragma once
#include "graphics.h"
#include "skeletal_mesh.h"
class SkyBox
{
public:
	SkyBox();
	~SkyBox() {}

	void render();

private:
	//環境マップとキューブマップのテクスチャの生成
	void load_convert_env_map_and_cube_map();
	//環境マップ計算
	void Compute_pre_filtered_spec_env_map();
	//
	std::unique_ptr<SkeletalMesh> sky_model;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT3 scale;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> env_map;
};
