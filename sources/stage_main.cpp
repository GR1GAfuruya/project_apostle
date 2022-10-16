#include "stage_main.h"

#include "user.h"

static StageMain* instance = nullptr;
//コンストラクタ
StageMain::StageMain(Graphics& graphics)
{
	instance = this;
	//ステージモデルを読み込み
	//model = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), ".\\resources\\Model\\Stage\\Cave.fbx", 1);
	model = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), ".\\resources\\Model\\Stage\\stage_hall.fbx", 1);
	model_collision = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), ".\\resources\\Model\\Stage\\stage_hall_collision.fbx", 1);
	scale = { 0.2f, 0.2f, 0.2f };
}

StageMain::~StageMain()
{
	
}

StageMain& StageMain::Instance()
{
	return *instance;
}

//更新処理
void StageMain::update(float elapsedTime)
{

}

void StageMain::render(float elapsed_time, Graphics& graphics)
{
	static DirectX::XMFLOAT4 material_color = { 1,1,1,1 };
	
	
	DirectX::XMFLOAT4X4 world = Math::calc_world_matrix(scale, angle, position);
	const DirectX::XMFLOAT4X4 terrain_world_transform{ -0.01f, 0.0f, 0.0f, 0.0f, 0.0f, 0.01f, 0.0f, 0.0f, 0.0f, 0.0f, 0.01f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

	//ステージ
	graphics.shader->render(graphics.get_dc().Get(), model.get(), world);
#if USE_IMGUI
	
	ImGui::Begin("stage");
	ImGui::DragFloat3("scale", &scale.x,0.1f);
	ImGui::End();
	
#endif
}

bool StageMain::ray_cast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	
	DirectX::XMFLOAT4X4 world = Math::calc_world_matrix(scale, angle, position);
	return Collision::ray_vs_model(start, end, model_collision.get(), world,hit);
}