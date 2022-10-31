#pragma once
#include "scene.h"
#include "sprite_batch.h"
#include "player.h"
#include "boss.h"
#include "stage.h"
#include "camera.h"
#include "post_effects.h"
#include "gpu_particle.h"
#include "particle.h"
#include "lambert_shader.h"
#include "deferred_renderer.h"
#include "light_manager.h"
#include "sky_box.h"
#include "field_spark_particle.h"
#include "generic_frequent_entities.h"
class SceneGame :public Scene, public GenericFrequentEntities
{
public:
	SceneGame(Graphics& graphics);
	~SceneGame()override {}

	void initialize(Graphics& graphics) override;
	void finalize() override;
	void update(float elapsedTime, Graphics& graphics) override;
	void render(float elapsedTime, Graphics& graphics) override;
	void debug_gui();
	void scene_reset();
private:
	std::unique_ptr<Camera> camera = nullptr;
	std::unique_ptr<Player> player = nullptr;
	std::unique_ptr<Boss> boss = nullptr;
	std::unique_ptr<Stage> stage = nullptr;
	DirectX::XMFLOAT3 chara_pos{};
	std::unique_ptr<PostEffects> post_effect;
	std::unique_ptr<field_spark_particles> field_spark_particle;
	std::unique_ptr<LambertShader> lambert_shader;
	std::unique_ptr<Particles> particles;
	std::unique_ptr<DeferredRenderer> deferred;
	std::unique_ptr<SkyBox> skybox;
	std::unique_ptr<SpriteBatch> operation_ui;

	//テスト用
#if _DEBUG
	std::unique_ptr<MeshEffect> test_mesh_effect;
	
#endif
};