#pragma once
#include <d3d11_1.h>
#include <dxgi1_6.h>

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <wrl.h>
#include<memory>
#include "misc.h"
#include "high_resolution_timer.h"

#include "device.h"
#include "scene_manager.h"
#ifdef USE_IMGUI
#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_internal.h"
#include "../external/imgui/imgui_impl_dx11.h"
#include "../external/imgui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif


CONST BOOL FULLSCREEN{ FALSE };
CONST LPWSTR APPLICATION_NAME{ LPWSTR(L"PROJECT_APOSTLE") };

class framework
{
public:
	CONST HWND hwnd;
	enum class PS { LUMINANCE, GAUTH_BLUR, NOIZE, RADIAL_BLUR };
	//オブジェクト宣言
	
	void select_pixel_shader(ID3D11Device* device, int select_id = 0);

	framework(HWND hwnd);
	~framework();

	framework(const framework&) = delete;
	framework& operator=(const framework&) = delete;
	framework(framework&&) noexcept = delete;
	framework& operator=(framework&&) noexcept = delete;
	/*float ang = 0;*/
		bool initialize();
		void update(float elapsed_time/*Elapsed seconds from last frame*/);

		BOOL get_file_name(HWND hWnd, TCHAR* fname, int sz, TCHAR* initDir);

		//void move_x();
		//void move_y();
		//typedef  void (framework::* P_hoge)();
		//P_hoge p_hoge = &framework::move_x;
		void render(float elapsed_time/*Elapsed seconds from last frame*/);
		bool uninitialize();
	int run();

	LRESULT  handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	high_resolution_timer tictoc;
	uint32_t frames{ 0 };
	float elapsed_time{ 0.0f };
	void calculate_frame_stats()
	{
		if (++frames, (tictoc.time_stamp() - elapsed_time) >= 1.0f)
		{
			float fps = static_cast<float>(frames);
			std::wostringstream outs;
			outs.precision(6);
			outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
			SetWindowTextW(hwnd, outs.str().c_str());

			frames = 0;
			elapsed_time += 1.0f;
		}
	}
private:
	//----------<変数>----------//
	std::unique_ptr<Graphics> graphics;
};

