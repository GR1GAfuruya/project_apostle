#include "skill_lightning_rain.h"
#include "Operators.h"
LightningRain::LightningRain(Graphics& graphics, DirectX::XMFLOAT3 launch_pos, DirectX::XMFLOAT3 target_pos, PublicParam initparam)
{
	lightning_mesh = std::make_unique<MeshEffect>(graphics, "./resources/Effects/Meshes/lightning.fbx");
	param = initparam;
	position = target_pos;
	attack_colider.start = position;
	//ìñÇΩÇËîªíËèâä˙âª
	skill_duration = 2.0f;
	DirectX::XMFLOAT3 lightning_dir = { 0,1,0 };
	attack_colider.end = position + Math::vector_scale(lightning_dir, param.lightning_length);
	attack_colider.radius = 0;
}

LightningRain::~LightningRain()
{
}

void LightningRain::initialize(Graphics& graphics)
{
}

void LightningRain::update(Graphics& graphics, float elapsed_time)
{
	if (life_time < 0.5f)
	{
		attack_colider.radius = param.collider_radius;
	}
	//è¡ñ≈èàóù
	life_time += elapsed_time;
	if (life_time > skill_duration) skill_end_flag = true;

}

void LightningRain::render(Graphics& graphics, Camera* camera)
{
	lightning_mesh->render(graphics, camera);
}

void LightningRain::lightning_emit(int index_offset, int emit_max_num, float size)
{
}

void LightningRain::debug_gui(string str_id)
{
}
