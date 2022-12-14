#pragma once
#include "graphics.h"
//シーン基底クラス
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	//初期化
	virtual void initialize(Graphics& graphics) = 0;

	//終了化
	virtual void finalize() = 0;

	//更新処理
	virtual void update(float elapsedTime,Graphics& graphics) = 0;

	//描画処理
	virtual void render(float elapsedTime,Graphics& graphics) = 0;

	//準備完了しているか
	bool is_ready() const { return ready; }

	//準備完了設定
	void set_ready(bool r) { ready = r; }

private:
	bool ready = false;
};
