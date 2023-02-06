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
	enum class TITLE_MENU
	{
		GAME_START,
		EXIT
	};
	//タイトル背景
	std::unique_ptr<SpriteBatch> sprite_title_back = nullptr;
	//タイトルロゴ
	std::unique_ptr<SpriteBatch> sprite_title_logo = nullptr;
	//タイトルロゴ背景
	std::unique_ptr<SpriteBatch> sprite_title_logo_back = nullptr;
	//スタート文字
	std::unique_ptr<SpriteBatch> sprite_start = nullptr;
	//EXIT文字
	std::unique_ptr<SpriteBatch> sprite_exit = nullptr;
	//選択バー
	std::unique_ptr<SpriteBatch> select_bar = nullptr;
	//カメラ
	std::unique_ptr<Camera> camera;

	DirectX::XMFLOAT2 select_bar_pos;

	TITLE_MENU selected_menu_state;
	//ロゴ背景に適応するピクセルシェーダー
	Microsoft::WRL::ComPtr<ID3D11PixelShader> logo_ps;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_main_color;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_mask;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_distortion;
};
