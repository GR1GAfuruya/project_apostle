#pragma once
#include "scene.h"
#include "sprite_batch.h"
#include "camera.h"
#include <memory>
class SceneTitle :public Scene
{
public:
	SceneTitle(Graphics& graphics);
	~SceneTitle()override {}

	void initialize(Graphics& graphics) override;
	void finalize() override;
	void update(float elapsedTime, Graphics& graphics) override;
	void render(float elapsedTime,Graphics& graphics) override;

private:
	//�^�C�g���w�i
	std::unique_ptr<SpriteBatch> sprite_title_back = nullptr;
	//�^�C�g�����S
	std::unique_ptr<SpriteBatch> sprite_title_logo = nullptr;
	//�^�C�g�����S�w�i
	std::unique_ptr<SpriteBatch> sprite_title_logo_back = nullptr;
	//�X�^�[�g����
	std::unique_ptr<SpriteBatch> sprite_start = nullptr;
	//EXIT����
	std::unique_ptr<SpriteBatch> sprite_exit = nullptr;
	//�J����
	std::unique_ptr<Camera> camera;

	//���S�w�i�ɓK������s�N�Z���V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11PixelShader> logo_ps;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_main_color;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_mask;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_distortion;
};
