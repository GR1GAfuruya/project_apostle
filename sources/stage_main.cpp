#include "stage_main.h"

#include "user.h"

static StageMain* instance = nullptr;
//==============================================================
// 
//コンストラクタ
// 
//==============================================================
StageMain::StageMain(Graphics& graphics)
{
	instance = this;
	//ステージモデルを読み込み
	//model = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), ".\\resources\\Model\\Stage\\Cave.fbx", 1);
	model = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), ".\\resources\\Model\\Stage\\stage_hall.fbx", 1);
	model_collision = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), ".\\resources\\Model\\Stage\\stage_hall_collision.fbx", 1);
	model_shadow = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), ".\\resources\\Model\\Stage\\stage_hall_shadow.fbx", 1);
	scale = { 0.2f, 0.2f, 0.2f };
}
//==============================================================
// 
//デストラクタ
// 
//==============================================================
StageMain::~StageMain()
{
	
}

StageMain& StageMain::Instance()
{
	return *instance;
}

//==============================================================
// 
//更新
// 
//==============================================================
void StageMain::update(float elapsedTime)
{
	transform = Math::calc_world_matrix(scale, angle, position);
}
//==============================================================
// 
//描画
// 
//==============================================================
void StageMain::render(Graphics& graphics, float elapsed_time, Camera* camera)
{
	static DirectX::XMFLOAT4 material_color = { 1,1,1,1 };
	
	
	

	//ステージ
	if (flustm_flag)
	{
		graphics.shader->render(graphics.get_dc().Get(), model.get(), camera->get_view(), camera->get_projection(), transform);
	}
	else
	{
		graphics.shader->render(graphics.get_dc().Get(), model.get(), transform);
	}
#if USE_IMGUI
	imgui_menu_bar("Stage", "stage_main", display_imgui);
	if (display_imgui)
	{

		ImGui::Begin("stage_main");
		ImGui::DragFloat3("scale", &scale.x, 0.1f);
		ImGui::Checkbox("flustm_flag", &flustm_flag);
		static int num = 0;
		ImGui::DragInt("mesh_num", &num, 1, 0, model.get()->model_resource.get()->get_meshes().size());
		int mesh_size = model.get()->model_resource.get()->get_meshes().size();
		ImGui::DragInt("mesh_size", &mesh_size);
		DirectX::XMFLOAT3 min = model.get()->model_resource.get()->get_meshes().at(num).bounding_box[0];
		DirectX::XMFLOAT3 max = model.get()->model_resource.get()->get_meshes().at(num).bounding_box[1];
		ImGui::DragFloat3("bounding_min", &min.x);
		ImGui::DragFloat3("bounding_max", &max.x);
		ImGui::End();
	}
#endif
}
void StageMain::shadow_render(Graphics& graphics, float elapsed_time)
{
	graphics.shader->render(graphics.get_dc().Get(), model_shadow.get(), transform);
}
//==============================================================
// 
//レイキャスト
// 
//==============================================================
bool StageMain::ray_cast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	
	return Collision::ray_vs_model(start, end, model_collision.get(), transform, hit);
}