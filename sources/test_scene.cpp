#include "test_scene.h"

#include "effect_manager.h"
#include "framework.h"
#include "imgui_include.h"
#include "scene_game.h"
#include "scene_loading.h"
#include "shader.h"
#include "texture.h"
#include "user.h"

SceneTest::SceneTest(Graphics& graphics)
{
	camera = std::make_unique<Camera>(graphics);
	//ステージモデルを読み込み
	model = std::make_unique<SkeletalMesh>(graphics.get_device().Get(), ".\\resources\\Model\\Cube.fbx", 1);
	scale = { 0.01f, 0.01f, 0.01f };
	sprite = std::make_unique<SpriteBatch>(graphics.get_device().Get(), L".\\resources\\Sprite\\chip_win.png",5);
	aura = std::make_unique<Aura>(graphics);
	//定数バッファ生成
	grass_constant_buffer = std::make_unique<Constants<grass_constants>>(graphics.get_device().Get());

	//シェーダー生成
	create_vs_from_cso(graphics.get_device().Get(), "./shaders/grass_vs.cso", vertex_shader.ReleaseAndGetAddressOf(),
		nullptr, nullptr, 0);

	create_hs_from_cso(graphics.get_device().Get(), "./shaders/grass_hs.cso", hull_shader.ReleaseAndGetAddressOf());
	create_ds_from_cso(graphics.get_device().Get(), "./shaders/grass_ds.cso", domain_shader.ReleaseAndGetAddressOf());
	create_gs_from_cso(graphics.get_device().Get(), "./shaders/grass_gs.cso", geometry_shader.ReleaseAndGetAddressOf());
	create_ps_from_cso(graphics.get_device().Get(), "./shaders/grass_ps.cso", pixel_shader.ReleaseAndGetAddressOf());
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	load_texture_from_file(graphics.get_device().Get(), L".\\resources\\TexMaps\\distortion texture.png", shader_resource_views.ReleaseAndGetAddressOf(), &texture2d_desc);
	particle = std::make_unique<GPU_Particles>(graphics.get_device().Get());


}
void SceneTest::initialize(Graphics& graphics)
{
}

void SceneTest::finalize()
{
}

void SceneTest::update(float elapsed_time, Graphics& graphics)
{
	/*ImGui::Text("accumulated  particle count %d", particle->particle_data.particle_count);
	ImGui::Text("accumulated  current_eye_position %d", particle->particle_data.current_eye_position);
	ImGui::Text("accumulated  particle_size %d", particle->particle_data.particle_size);
	ImGui::Text("accumulated  outermost_radius %d", particle->particle_data.outermost_radius);
	ImGui::Text("accumulated  snowfall_area_height %d", particle->particle_data.snowfall_area_height);*/

	camera->update(elapsed_time);
	camera->calc_view_projection(graphics, elapsed_time);

	EffectManager::Instance().update(graphics, elapsed_time);

}

void SceneTest::render(float elapsed_time, Graphics& graphics)
{
	DirectX::XMFLOAT4X4 world = Math::calc_world_matrix(scale, angle, position);
	const XMFLOAT4X4 terrain_world_transform{ -0.01f, 0.0f, 0.0f, 0.0f, 0.0f, 0.01f, 0.0f, 0.0f, 0.0f, 0.0f, 0.01f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::SOLID_ONESIDE);
	//model->render(graphics.get_dc().Get(), world);
	//graphics.set_graphic_state_priset(DEPTH_ST::ZT_ON_ZW_OFF, BLEND_ST::ALPHA, RASTERIZER_ST::CULL_NONE);
	//particle->render(graphics.get_dc().Get());
	/*Mouse& mouse = Device::instance().get_mouse();
	if (mouse.get_button() & mouse.BTN_Z)
	{
		SceneManager::instance().change_scene(graphics, new SceneLoading(new SceneGame(graphics)));
		return;
	}*/

	//graphics.set_graphic_state_priset(DEPTH_ST::ZT_ON_ZW_ON, BLEND_ST::ALPHA, RASTERIZER_ST::CULL_NONE);
	//model->render(graphics.get_dc().Get(), dummy_animeparam, terrain_world_transform,
	//	[&](const SkeletalMesh::mesh&, const SkeletalMesh::material& material,
	//		SkeletalMesh::shader_resources& shader_resources, SkeletalMesh::pipeline_state& pipeline_state) {

	//			graphics.get_dc()->UpdateSubresource(constant_buffer.Get(), 0, 0, &grass_data, 0, 0);
	//			graphics.get_dc()->HSSetConstantBuffers(4, 1, constant_buffer.GetAddressOf());
	//			graphics.get_dc()->DSSetConstantBuffers(4, 1, constant_buffer.GetAddressOf());
	//			graphics.get_dc()->GSSetConstantBuffers(4, 1, constant_buffer.GetAddressOf());
	//			graphics.get_dc()->PSSetConstantBuffers(4, 1, constant_buffer.GetAddressOf());

	//			graphics.get_dc()->GSSetShaderResources(5, 1, shader_resource_views.GetAddressOf());

	//			//各種ステート設定
	//			pipeline_state.depth_stencil_state = graphics.get_depth_stencil_state(DEPTH_ST::ZT_ON_ZW_ON).Get();
	//			pipeline_state.blend_state = graphics.get_blend_state(BLEND_ST::ALPHA).Get();
	//			pipeline_state.rasterizer_state = graphics.get_rasterizer_state(RASTERIZER_ST::CULL_NONE).Get();
	//			//--シェーダーオブジェクト--//
	//			//VS
	//			pipeline_state.vertex_shader = vertex_shader.Get();
	//			//HS
	//			pipeline_state.hull_shader = hull_shader.Get();
	//			//DS
	//			pipeline_state.domain_shader = domain_shader.Get();
	//			//GS
	//			pipeline_state.geometry_shader = geometry_shader.Get();
	//			// PS
	//			pipeline_state.pixel_shader = pixel_shader.Get();
	//			pipeline_state.topology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
	//			pipeline_state.stencil_ref = 1;
	//			return 0;
	//	});
	graphics.set_graphic_state_priset(ST_DEPTH::ZT_OFF_ZW_OFF, ST_BLEND::ALPHA, ST_RASTERIZER::CULL_NONE);
	aura->update(graphics, elapsed_time);
	sprite->begin(graphics.get_dc().Get(),aura.get()->pixel_shader.Get());
	DirectX::XMFLOAT2 tex_size = { static_cast<float>(SCREEN_WIDTH) /sprite->get_texture2d_desc().Width/2  ,static_cast<float>(SCREEN_HEIGHT) /sprite->get_texture2d_desc().Height/2 };
	sprite->render(graphics.get_dc().Get(), { 0,0 },{ tex_size });
	sprite->end(graphics.get_dc().Get());

	debug_gui();
}

void SceneTest::debug_gui()
{
#ifdef USE_IMGUI
	ImGui::Begin("Camera");
	ImGui::DragFloat3("pos", &position.x);
	ImGui::DragFloat3("angle", &angle.x);
	ImGui::DragFloat3("scale", &scale.x);
	ImGui::End();
	
#endif // USE_IMGUI
	camera->debug_gui();

}
