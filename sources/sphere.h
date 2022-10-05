#pragma once
#include "effect_base.h"
#include "skeletal_mesh.h"
#include "constant.h"
class SphereEffect : public EffecttBase
{
public:
	SphereEffect(ID3D11Device* device);
	~SphereEffect() {}
	void play(DirectX::XMFLOAT3 pos)override;
	void stop()override;
	void update(Graphics& graphics, float elapsed_time)override;
	void render(Graphics& graphics)override;
	void debug_gui(const char* str_id)override;

private:
	struct SPHERE_CONSTANTS
	{
		DirectX::XMFLOAT4 particle_color{0.0f,0.0f,0.0f,1.0f};
		DirectX::XMFLOAT2 scroll_direction{};
		float scroll_speed{};
		float thirethold{};
	};
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textures[2];

	// スケルタルメッシュの実体
	std::unique_ptr <SkeletalMesh> model;
public:
	std::unique_ptr<Constants<SPHERE_CONSTANTS>> constants{};
};
