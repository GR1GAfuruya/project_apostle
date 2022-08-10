#include "user.h"
#include "mouse.h"

static const int KeyMap[] =
{
	VK_LBUTTON,		// 左ボタン
	VK_MBUTTON,		// 中ボタン
	VK_RBUTTON,		// 右ボタン
	'W',
	'A',
	'S',
	'D',
	VK_SPACE,
	'M',
	VK_ESCAPE,
	'Z',
	'I',
	'K',
	VK_SHIFT,
	'J',
	'L',
	VK_RETURN,
	VK_UP,
	VK_LEFT,
	VK_DOWN,
	VK_RIGHT,
};

void Mouse::update(HWND hwnd)
{
	update_cursor(hwnd);
	//---キーボード----//
	// スイッチ情報
	MouseButton newButtonState = 0;

	for (int i = 0; i < ARRAYSIZE(KeyMap); ++i)
	{
		if (::GetAsyncKeyState(KeyMap[i]) & 0x8000)
		{
			newButtonState |= (1 << i);
		}
	}
	// ボタン情報更新
	if (operable)
	{
		button_state[1] = button_state[0];	// スイッチ履歴
		button_state[0] = newButtonState;

		button_down = ~button_state[1] & newButtonState;	// 押した瞬間
		button_up = ~newButtonState & button_state[1];	// 離した瞬間
	}
	else
	{
		button_state[1] = 0;	// スイッチ履歴
		button_state[0] = 0;

		button_down = 0;	// 押した瞬間
		button_up = 0;	// 離した瞬間
	}
}

void Mouse::update_cursor(HWND hwnd)
{
	// マウス
	GetCursorPos(&point);           // スクリーン座標を取得する
	ScreenToClient(hwnd, &point);   // クライアント座標に変換する
	old_cursor_pos = cursor_pos;
	cursor_pos.x = (float)point.x - 15.0f;
	cursor_pos.y = (float)point.y - 15.0f;
	//　カーソルの範囲
	cursor_pos.x = std::clamp(cursor_pos.x, 0.0f, 1280.0f);
	cursor_pos.y = std::clamp(cursor_pos.y, 0.0f, 720.0f);
	// マウスの操作
	switch (state)
	{
	case 0: // 初期化
		SetCursorPos((int)cursor_pos.x, (int)cursor_pos.y);
		++state;
		/*fallthrough*/
	case 1: // 通常時
		ShowCursor(do_show);
		break;
	}
}