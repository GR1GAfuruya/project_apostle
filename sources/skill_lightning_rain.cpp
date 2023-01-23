#include "skill_lightning_rain.h"

LightningRain::LightningRain(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos, PublicParam initparam)
{
	lightning_mesh = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/lightning.fbx");
}

LightningRain::~LightningRain()
{
}

void LightningRain::initialize(Graphics& graphics)
{
}

void LightningRain::update(Graphics& graphics, float elapsed_time)
{
}

void LightningRain::render(Graphics& graphics)
{
	lightning_mesh->render(graphics)
}

void LightningRain::lightning_emit(int index_offset, int emit_max_num, float size)
{
}

void LightningRain::debug_gui(string str_id)
{
}
