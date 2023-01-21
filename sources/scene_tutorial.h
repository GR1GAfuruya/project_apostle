#pragma once
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
#include "tutorial.h"
#include "emitter.h"
class SceneTutorial :public Scene, public GenericFrequentEntities
{
public:
	SceneTutorial(Graphics& graphics);
	~SceneTutorial()override {}

	void initialize(Graphics& graphics) override;
	void finalize() override;
	void update(float elapsed_time, Graphics& graphics) override;
	void render(float elapsed_time, Graphics& graphics) override;
	void debug_gui();
	void scene_reset();
private:
	//カメラ
	std::unique_ptr<Camera> camera = nullptr;
	//プレイヤー
	std::unique_ptr<Player> player = nullptr;
	//キャラ初期位置
	DirectX::XMFLOAT3 chara_pos{};
	//ディファードレンダー
	std::unique_ptr<DeferredRenderer> deferred = nullptr;
	//スカイボックス
	std::unique_ptr<SkyBox> skybox = nullptr;
	//操作説明UI（仮）
	std::unique_ptr<SpriteBatch> operation_ui = nullptr;
	//平行光
	std::shared_ptr<DirectionalLight> dir_light = nullptr;

	std::unique_ptr<Tutorial> tutorial = nullptr;
	//タイトルに戻る　※テスト用
	bool display_imgui = false;
};