#pragma once
#include "graphics.h"
#include "camera.h"
#include "collision.h"
class Camera;
 
class Stage
{
public:
	Stage(){}
	virtual ~Stage(){}

	virtual void  update(float elapsedTime) = 0;

	virtual void  render(Graphics& graphics, float elapsed_time,Camera* camera ) = 0;

	virtual bool ray_cast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

	bool display_imgui = false;
};