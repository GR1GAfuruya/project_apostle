#include "debug_flag.h"
#include "device.h"
bool DebugFlag::perspective_switching = false;
bool DebugFlag::wireframe_switching = false;
bool DebugFlag::debug_2D_switching = false;

void DebugFlag::update()
{
    if (Device::instance().get_mouse().get_button_down() & Mouse::BTN_F1) { perspective_switching ? perspective_switching = false : perspective_switching = true; }
    if (Device::instance().get_mouse().get_button_down() & Mouse::BTN_F2) { wireframe_switching ? wireframe_switching = false : wireframe_switching = true; }
    if (Device::instance().get_mouse().get_button_down() & Mouse::BTN_F3) { debug_2D_switching ? debug_2D_switching = false : debug_2D_switching = true; }
}