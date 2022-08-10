#include "device.h"

Device::Device()
{
}



void Device::update(HWND hwnd, float elapsed_time)
{
	mouse.update(hwnd);
	game_pad.update(elapsed_time);
}
