#pragma once
#include "graphics.h"
#include "Collision.h"
class Stage
{
public:
	Stage(){}
	virtual ~Stage(){}

	virtual void  update(float elapsedTime) = 0;

	virtual void  render(float elapsed_time, Graphics& graphics) = 0;

	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

};