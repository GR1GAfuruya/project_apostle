#pragma once
#include "constant.h"
#include "effect_base.h"
#include "skeletal_mesh.h"
class Aura: public EffecttBase
{
public:
	Aura(ID3D11Device* device);
	~Aura(){}
	void play(DirectX::XMFLOAT3 pos);
	void stop();
	void update(Graphics& graphics,float elapsed_time);
	void render(Graphics& graphics);
private:
	struct AURA_CONSTANTS
	{
		DirectX::XMFLOAT4 particle_color = {0,0,0,1};
		DirectX::XMFLOAT2 scroll_direction;
		float scroll_speed;
		float thirethold;
	};
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[2];

	// スケルタルメッシュの実体
	std::unique_ptr <SkeletalMesh> model;
public:
	std::unique_ptr<Constants<AURA_CONSTANTS>> aura_constants{};
	float rot_speed = 0;
};
