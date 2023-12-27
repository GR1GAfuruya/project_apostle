#pragma once
#include "component.h"
class ParticleSystem :public Component
{
public:
	ParticleSystem(const char* file_path);
	virtual ~ParticleSystem();

	void Launch(DirectX::XMFLOAT3 pos);

	void update(float elapsed_time);

	const char* get_name() const { return "ParticleSystem"; }

	void on_gui()override;
private:
	void register_emitter(std::shared_ptr<GameObject> p);

	std::vector<std::shared_ptr<GameObject>> emitters;

	//パーティクル全体の期間
	float duration;

	float time = 0;
	bool active = false;
	DirectX::XMFLOAT3	position = { 0, 0, 0 };
	DirectX::XMFLOAT3	scale = { 1, 1, 1 };
	DirectX::XMFLOAT4 orientation{ 0,0,0,1 };
	DirectX::XMFLOAT4X4	transform = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	};
};