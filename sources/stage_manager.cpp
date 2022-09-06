#include "stage_manager.h"

//XVˆ—
void StageManager::update(float elapsedTime)
{
	for(auto s : stages)
	{
		s->update(elapsedTime);
	}
}

void StageManager::render(float elapsed_time, Graphics& graphics)
{
	for (auto s : stages)
	{
		s->render(elapsed_time, graphics);
	}}

void StageManager::Clear()
{
	for (auto s : stages)
	{
		delete s;
	}
	stages.clear();
}

void StageManager::Register(class Stage* stage)
{
	this->stages.emplace_back(stage);
}



bool StageManager::ray_cast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;
	hit.distance = FLT_MAX;
	for (auto s : stages)
	{
		HitResult hr;
		if(s->ray_cast(start, end, hr))
		{
			hit = hr;
			return true;
		}
	}
	return result;
}
