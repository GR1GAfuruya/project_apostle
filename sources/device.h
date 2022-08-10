#pragma once
#include "game_pad.h"
#include "mouse.h"
#include <memory>

class Device
{
private:
	Device();
	~Device() {}
public:
	static Device& instance()
	{
		static Device instance;
		return instance;
	}
	// 更新処理
	void update(HWND hwnd,float elapsed_time);

	// ゲームパッド取得
	GamePad& get_game_pad() { return game_pad; }

	// マウス取得
	Mouse& get_mouse() { return mouse; }
private:
	GamePad game_pad;
	Mouse mouse;

};
