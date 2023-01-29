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
	//タイトル背景
	std::unique_ptr<SpriteBatch> title_back = nullptr;
	//タイトルロゴ
	std::unique_ptr<SpriteBatch> title_logo = nullptr;
	//タイトルロゴ背景
	std::unique_ptr<SpriteBatch> title_logo_back = nullptr;
	//カメラ
	std::unique_ptr<Camera> camera;

	//ロゴ背景に適応するピクセルシェーダー
	Microsoft::WRL::ComPtr<ID3D11PixelShader> logo_ps;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_main_color;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_mask;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_distortion;
};
