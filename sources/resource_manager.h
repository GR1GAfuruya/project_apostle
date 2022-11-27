#pragma once

#include <memory>
#include <string>
#include <map>
#include "model_resource.h"
#include "sprite_batch.h"
#include "material.h"
class ResourceManager
{
public:
	ResourceManager() {}
	~ResourceManager() {}
	// モデルリソース読み込み
	std::shared_ptr<ModelResource> load_model_resource(ID3D11Device* device, const char* fbx_filename, bool triangulate = false, float sampling_rate = 0);
	//// スプライトリソース読み込み
	//std::shared_ptr<Material> load_material_resource(Graphics& graphics, string id);


	static ResourceManager& instance()
	{
		static ResourceManager instance;
		return instance;
	}
private:
	// model
	using ModelMap = std::map<std::string, std::weak_ptr<ModelResource>>;
	ModelMap  models;
	// sprite
	//using MaterialMap = std::map<std::string, std::weak_ptr<Material>>;
	//MaterialMap materials;
};