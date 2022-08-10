#pragma once

using GamePadButton = unsigned int;

// �Q�[���p�b�h
class GamePad
{
public:
	static const GamePadButton BTN_UP             = (1 << 0);
	static const GamePadButton BTN_RIGHT          = (1 << 1);
	static const GamePadButton BTN_DOWN           = (1 << 2);
	static const GamePadButton BTN_LEFT           = (1 << 3);
	static const GamePadButton BTN_A              = (1 << 4);
	static const GamePadButton BTN_B              = (1 << 5);
	static const GamePadButton BTN_X              = (1 << 6);
	static const GamePadButton BTN_Y              = (1 << 7);
	static const GamePadButton BTN_START          = (1 << 8);
	static const GamePadButton BTN_BACK           = (1 << 9);
	static const GamePadButton BTN_LEFT_THUMB     = (1 << 10);
	static const GamePadButton BTN_RIGHT_THUMB    = (1 << 11);
	static const GamePadButton BTN_LEFT_SHOULDER  = (1 << 12);
	static const GamePadButton BTN_RIGHT_SHOULDER = (1 << 13);
	static const GamePadButton BTN_LEFT_TRIGGER   = (1 << 14);
	static const GamePadButton BTN_RIGHT_TRIGGER  = (1 << 15);
public:
	GamePad() {}
	~GamePad() {}
	// �X�V
	/*�Ăяo����Framework�̂�*/
	void update(float elapsedTime);
	// �X���b�g�ݒ�
	void set_slot(int slot) { this->slot = slot; }
	// �{�^�����͏�Ԃ̎擾
	GamePadButton get_button() const { return button_state[0]; }
	// �{�^��������Ԃ̎擾
	GamePadButton get_button_down() const { return button_down; }
	// �{�^�������Ԃ̎擾
	GamePadButton get_button_up() const { return button_up; }
	// ���X�e�B�b�NX�����͏�Ԃ̎擾
	float get_axis_LX() const { return axisLx; }
	// ���X�e�B�b�NY�����͏�Ԃ̎擾
	float get_axis_LY() const { return axisLy; }
	// �E�X�e�B�b�NX�����͏�Ԃ̎擾
	float get_axis_RX() const { return axisRx; }
	// �E�X�e�B�b�NY�����͏�Ԃ̎擾
	float get_axis_RY() const { return axisRy; }
	// ���g���K�[���͏�Ԃ̎擾
	float get_trigger_L() const { return triggerL; }
	// �E�g���K�[���͏�Ԃ̎擾
	float get_trigger_R() const { return triggerR; }
	// �o�C�u���[�V����
	bool set_vibration(float R, float L, float StopTime);
	// ����L���A����
	void operation_disablement() { operable = false; }
	void operation_activation() { operable = true; }
private:
	GamePadButton		button_state[2] = { 0 };
	GamePadButton		button_down = 0;
	GamePadButton		button_up = 0;
	float				axisLx = 0.0f;
	float				axisLy = 0.0f;
	float				axisRx = 0.0f;
	float				axisRy = 0.0f;
	float				triggerL = 0.0f;
	float				triggerR = 0.0f;
	int					slot = 0;

	float stack_stop_vib_sec;
	float vib_stack_sec;

	bool operable = true;
};