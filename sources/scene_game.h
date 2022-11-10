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
	//�J����
	std::unique_ptr<Camera> camera = nullptr;
	//�v���C���[
	std::unique_ptr<Player> player = nullptr;
	//�{�X
	std::unique_ptr<Boss> boss = nullptr;
	//�L���������ʒu
	DirectX::XMFLOAT3 chara_pos{};
	//�|�X�g�G�t�F�N�g
	std::unique_ptr<PostEffects> post_effect;
	//�t�B�[���h��̉Ή�
	std::unique_ptr<field_spark_particles> field_spark_particle;
	std::unique_ptr<Particles> particles;
	//�f�B�t�@�[�h�����_�[
	std::unique_ptr<DeferredRenderer> deferred;
	//�X�J�C�{�b�N�X
	std::unique_ptr<SkyBox> skybox;
	//�������UI�i���j
	std::unique_ptr<SpriteBatch> operation_ui;
	//���s��
	std::shared_ptr<DirectionalLight> dir_light;
	//�e�X�g�p
#if _DEBUG
	std::unique_ptr<MeshEffect> test_mesh_effect;
	//std::unique_ptr<Meteore> test_meteore;
	std::unique_ptr<EffekseerEffect> test_meteore;
	
#endif
};