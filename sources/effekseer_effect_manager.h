#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include "graphics.h"
#include "camera.h"

//エフェクトマネージャー
class EffekseerEffectManager
{
private:
	EffekseerEffectManager(){}
	~EffekseerEffectManager(){}

public:
	static EffekseerEffectManager& Instance()
	{
		static EffekseerEffectManager instance;
		return instance;
	}

	//初期化
	void initialize(Graphics& graphics);
	//終了化
	void finalize();
	//更新処理
	void update(Graphics& graphics, float elapsed_time);
	//描画処理
	void render(Camera& camera);
	
	//Effekseerマネージャーの取得
	Effekseer::ManagerRef get_effekseer_manager() {return effekseer_manager;}
	

private:
	Effekseer::ManagerRef effekseer_manager = nullptr;
	EffekseerRenderer::RendererRef effekseer_renderer = nullptr;
};
