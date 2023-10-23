#pragma once
#include "graphics.h"
//シーン基底クラス
class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	//初期化
	virtual void initialize() = 0;

	//終了化
	virtual void finalize() = 0;

	//更新処理
	virtual void update(float elapsedTime) = 0;

	//描画処理
	virtual void render(float elapsedTime) = 0;

	//準備完了しているか
	bool is_ready() const { return ready; }

	//準備完了設定
	void set_ready(bool r) { ready = r; }

private:
	bool ready = false;
protected:
	bool is_change_scean_start;
};
