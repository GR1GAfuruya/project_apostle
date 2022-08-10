#include "effekseer_effect_manager.h"
//初期化
void EffekseerEffectManager::initialize(Graphics& graphics)
{
	// エフェクトのレンダラーの作成
	effekseer_renderer = EffekseerRendererDX11::Renderer::Create(graphics.get_device().Get(),
		graphics.get_dc().Get(), 2048);

	// エフェクトのマネージャーの作成
	effekseer_manager = Effekseer::Manager::Create(2048);

	// 描画モジュールの設定
	effekseer_manager->SetSpriteRenderer(effekseer_renderer->CreateSpriteRenderer());
	effekseer_manager->SetRibbonRenderer(effekseer_renderer->CreateRibbonRenderer());
	effekseer_manager->SetRingRenderer(effekseer_renderer->CreateRingRenderer());
	effekseer_manager->SetTrackRenderer(effekseer_renderer->CreateTrackRenderer());
	effekseer_manager->SetModelRenderer(effekseer_renderer->CreateModelRenderer());

	// テクスチャ、モデル、カーブ、マテリアルローダーの設定する。
	// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	effekseer_manager->SetTextureLoader(effekseer_renderer->CreateTextureLoader());
	effekseer_manager->SetModelLoader(effekseer_renderer->CreateModelLoader());
	effekseer_manager->SetMaterialLoader(effekseer_renderer->CreateMaterialLoader());
	effekseer_manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	//Effekseerを左手座標系で計算する
	effekseer_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

void EffekseerEffectManager::finalize()
{
	if(effekseer_manager.Get() != NULL)
	{
		effekseer_manager.Reset();
	}

	if (effekseer_renderer.Get() != NULL)
	{
		effekseer_renderer.Reset();
	}
}

void EffekseerEffectManager::update(Graphics& graphics, float elapsed_time)
{
	effekseer_manager->Update(elapsed_time * 60.0f);
}

void EffekseerEffectManager::render(Camera& camera)
{
	//ビュー＆プロジェクション行列をEffekseerレンダラに設定
	effekseer_renderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&camera.get_view()));
	effekseer_renderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&camera.get_projection()));

	//描画開始
	effekseer_renderer->BeginRendering();

	//描画実行
	effekseer_manager->Draw();

	//描画終了
	effekseer_renderer->EndRendering();
}
