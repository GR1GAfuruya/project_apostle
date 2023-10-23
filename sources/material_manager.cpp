#include "material_manager.h"

void MaterialManager::create_materials()
{
	mat_fire_distortion = make_shared<FireDistortionMat>();
	mat_meteore = make_shared<MeteoreMat>();
	mat_lightning = make_shared<LightningMat>();
}
