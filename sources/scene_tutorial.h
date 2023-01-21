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
	//�J����
	std::unique_ptr<Camera> camera = nullptr;
	//�v���C���[
	std::unique_ptr<Player> player = nullptr;
	//�L���������ʒu
	DirectX::XMFLOAT3 chara_pos{};
	//�f�B�t�@�[�h�����_�[
	std::unique_ptr<DeferredRenderer> deferred = nullptr;
	//�X�J�C�{�b�N�X
	std::unique_ptr<SkyBox> skybox = nullptr;
	//�������UI�i���j
	std::unique_ptr<SpriteBatch> operation_ui = nullptr;
	//���s��
	std::shared_ptr<DirectionalLight> dir_light = nullptr;

	std::unique_ptr<Tutorial> tutorial = nullptr;
	//�^�C�g���ɖ߂�@���e�X�g�p
	bool display_imgui = false;
};