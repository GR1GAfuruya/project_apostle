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
	// �X�V����
	void update(HWND hwnd,float elapsed_time);

	// �Q�[���p�b�h�擾
	GamePad& get_game_pad() { return game_pad; }

	// �}�E�X�擾
	Mouse& get_mouse() { return mouse; }
private:
	GamePad game_pad;
	Mouse mouse;

};
