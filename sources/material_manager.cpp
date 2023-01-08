#include "material_manager.h"

void MaterialManager::create_materials(Graphics& graohics)
{
	mat_fire_distortion = make_shared<FireDistortionMat>(graohics);
	mat_meteore = make_shared<MeteoreMat>(graohics);
	mat_lightning = make_shared<LightningMat>(graohics);
}
