#include "user.h"
#include "mouse.h"

static const int KeyMap[] =
{
	VK_LBUTTON,		// ���{�^��
	VK_MBUTTON,		// ���{�^��
	VK_RBUTTON,		// �E�{�^��
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
	//---�L�[�{�[�h----//
	// �X�C�b�`���
	MouseButton newButtonState = 0;

	for (int i = 0; i < ARRAYSIZE(KeyMap); ++i)
	{
		if (::GetAsyncKeyState(KeyMap[i]) & 0x8000)
		{
			newButtonState |= (1 << i);
		}
	}
	// �{�^�����X�V
	if (operable)
	{
		button_state[1] = button_state[0];	// �X�C�b�`����
		button_state[0] = newButtonState;

		button_down = ~button_state[1] & newButtonState;	// �������u��
		button_up = ~newButtonState & button_state[1];	// �������u��
	}
	else
	{
		button_state[1] = 0;	// �X�C�b�`����
		button_state[0] = 0;

		button_down = 0;	// �������u��
		button_up = 0;	// �������u��
	}
}

void Mouse::update_cursor(HWND hwnd)
{
	// �}�E�X
	GetCursorPos(&point);           // �X�N���[�����W���擾����
	ScreenToClient(hwnd, &point);   // �N���C�A���g���W�ɕϊ�����
	old_cursor_pos = cursor_pos;
	cursor_pos.x = (float)point.x - 15.0f;
	cursor_pos.y = (float)point.y - 15.0f;
	//�@�J�[�\���͈̔�
	cursor_pos.x = std::clamp(cursor_pos.x, 0.0f, 1280.0f);
	cursor_pos.y = std::clamp(cursor_pos.y, 0.0f, 720.0f);
	// �}�E�X�̑���
	switch (state)
	{
	case 0: // ������
		SetCursorPos((int)cursor_pos.x, (int)cursor_pos.y);
		++state;
		/*fallthrough*/
	case 1: // �ʏ펞
		ShowCursor(do_show);
		break;
	}
}