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
#include "effect.h"
class SceneGame :public Scene, public GenericFrequentEntities
{
public:
	SceneGame();
	~SceneGame()override {}
	//シーン初期化
	void initialize() override;
	//シーン終了処理
	void finalize() override;
	//シーンアップデート
	void update(float elapsedTime) override;
	//シーン描画
	void render(float elapsedTime) override;
	//クリ時の更新
	void clear_update(float elapsedTime);
	//デバッグ描画
	void debug_gui();
	//シーンリセット
	void scene_reset();
private:
	//カメラ
	std::unique_ptr<Camera> camera = nullptr;
	//プレイヤー
	std::unique_ptr<Player> player = nullptr;
	//ボス
	std::unique_ptr<Boss> boss = nullptr;
	//キャラ初期位置
	DirectX::XMFLOAT3 chara_pos{};
	//フィールド上の火花
	std::unique_ptr<field_spark_particles> field_spark_particle = nullptr;
	std::unique_ptr<Particles> particles = nullptr;
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
	//テスト用
#if _DEBUG
	std::unique_ptr<MeshEffect> test_mesh_effect = nullptr;
	std::unique_ptr<MeshEffect> test_mesh_effect2 = nullptr;
	std::unique_ptr<MeshEffect> test_mesh_effect3 = nullptr;
	std::unique_ptr<Effect> test_effect = nullptr;
#endif
};