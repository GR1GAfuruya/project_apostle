#include "framework.h"
#include "scene_title.h"
#include <array>
#include <d3dcompiler.h>
#include <string>


framework::framework(HWND hwnd) : hwnd(hwnd)
{
	
}

bool framework::initialize()
{

	graphics = std::make_unique<Graphics>();

	graphics->initialize(hwnd);
	entities_initialize(*graphics);

	// debug_flags
	debug_flags = std::make_unique<DebugFlag>();
	SceneManager::instance().change_scene(*graphics, new SceneTitle(*graphics));
	return true;
}

void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
	
	// device
	Device::instance().update(hwnd, elapsed_time);
	Device::instance().get_mouse().operation_activation();
	Device::instance().get_game_pad().operation_activation();

	debug_flags->update();
	//シーンアップデート
	graphics->set_hwnd(hwnd);
	SceneManager::instance().update(elapsed_time,*graphics);
	graphics->debug_gui();

	// フレーム表示
	{
		ImGui::Begin("##frame rate");

		static float temp_value = 0;
		static float values[90] = {};
		static int values_offset = 0;
		static float refresh_time = 0.0f;
		static const float PLOT_SENSE = 0.2f;

		refresh_time += elapsed_time;
		if (static_cast<int>(refresh_time / PLOT_SENSE) >= 1)
		{
			values_offset = values_offset >= IM_ARRAYSIZE(values) ? 0 : values_offset;
			values[values_offset] = temp_value = elapsed_time * 1000.0f;

			++values_offset;
			refresh_time = 0;
		}

		char overlay[32];
		sprintf_s(overlay, "now: %d fps  %.3f ms", static_cast<int>(1000.0f / temp_value), temp_value);
		ImGui::PlotLines("##frame", values, IM_ARRAYSIZE(values), values_offset, overlay, 0, 20, ImVec2(ImGui::GetWindowSize().x * 0.95f, ImGui::GetWindowSize().y * 0.7f));

		ImGui::End();
	}
}

BOOL framework::get_file_name(HWND hWnd, TCHAR* fname, int sz, TCHAR* initDir)
{
	OPENFILENAMEW o;
	fname[0] = _T('\0');
	ZeroMemory(&o, sizeof(o));
	o.lStructSize = sizeof(o);          // 構造体サイズ
	o.hwndOwner = hWnd;               // 親ウィンドウのハンドル
	o.lpstrInitialDir = initDir;            // 初期フォルダー
	o.lpstrFile = fname;              // 取得したファイル名を保存するバッファ
	o.nMaxFile = sz;                 // 取得したファイル名を保存するバッファサイズ
	o.lpstrFilter = _TEXT("hlslファイル(*.hlsl)\0*.hlsl\0") _TEXT("全てのファイル(*.*)\0*.*\0");
	o.lpstrDefExt = _TEXT("hlsl");
	o.lpstrTitle = _TEXT("hlslファイルを指定");
	o.nFilterIndex = 1;
	return GetOpenFileNameW(&o);
}

void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	//別スレッド中にデバイスコンテキストが使われていた場合に
	//同時アクセスしないように排他制御する
	std::lock_guard<std::mutex> lock(graphics->get_mutex());
	{
		ID3D11RenderTargetView* null_render_target_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
		graphics->get_dc()->OMSetRenderTargets(_countof(null_render_target_views), null_render_target_views, 0);
		ID3D11ShaderResourceView* null_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
		graphics->get_dc()->VSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
		graphics->get_dc()->PSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);

		FLOAT color[]{ 0.2f, 0.2f, 0.2f, 1.0f };
		graphics->get_dc()->ClearRenderTargetView(graphics->get_render_target_view().Get(), color);
		graphics->get_dc()->ClearDepthStencilView(graphics->get_depth_stencil_view().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		graphics->get_dc()->OMSetRenderTargets(1, graphics->get_render_target_view().GetAddressOf(), graphics->get_depth_stencil_view().Get());

		graphics->get_dc()->PSSetSamplers(0, 1, graphics->get_sampler_state(ST_SAMPLER::POINT).GetAddressOf());
		graphics->get_dc()->PSSetSamplers(1, 1, graphics->get_sampler_state(ST_SAMPLER::LINEAR).GetAddressOf());
		graphics->get_dc()->PSSetSamplers(2, 1, graphics->get_sampler_state(ST_SAMPLER::ANISOTROPIC).GetAddressOf());
		graphics->get_dc()->PSSetSamplers(3, 1, graphics->get_sampler_state(ST_SAMPLER::LINEAR_BORDER_BLACK).GetAddressOf());
		graphics->get_dc()->PSSetSamplers(4, 1, graphics->get_sampler_state(ST_SAMPLER::LINEAR_BORDER_WHITE).GetAddressOf());
		graphics->get_dc()->PSSetSamplers(5, 1, graphics->get_sampler_state(ST_SAMPLER::CLAMP).GetAddressOf());
		graphics->get_dc()->PSSetSamplers(6, 1, graphics->get_sampler_state(ST_SAMPLER::SHADOW_MAP).GetAddressOf());

		graphics->set_graphic_state_priset(ST_DEPTH::ZT_ON_ZW_ON, ST_BLEND::ALPHA, ST_RASTERIZER::SOLID_ONESIDE );
	}


	SceneManager::instance().render(elapsed_time,*graphics);
#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
	UINT sync_interval{ 0 };
	graphics->get_swap_chain()->Present(sync_interval, 0);
}

LRESULT framework::handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
#ifdef USE_IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif
	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps{};
		BeginPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)
		{
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
		break;
	case WM_ENTERSIZEMOVE:
		tictoc.stop();
		break;
	case WM_EXITSIZEMOVE:
		tictoc.start();
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}


bool framework::uninitialize()
{
	
	//releaseAllTextures();
	SceneManager::instance().clear();
	entities_uninitialize(*graphics);

	return true;
}


int framework::run()
{
	MSG msg{};

	if (!initialize())
	{
		uninitialize();
		return 0;
	}

#ifdef USE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);
	// imgui flagsの初期化
	{
		//Imflags_preset_1 |= ImGuiWindowFlags_NoMove;
	}
	ImGuiIO& io = ImGui::GetIO();
	//----ここで設定しているConfigFlagsは全体に適応される----//
	// imgui ドッキングのフラグ    // Enable docking(available in imgui `docking` branch at the moment)
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// メインウィンドウの外に出す  // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	io.ConfigViewportsNoDecoration = true; // imguiウィンドウをメインウィンドウの外に出したとき全画面、最小化ができる: false
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(graphics->get_device().Get(), graphics->get_dc().Get());
	ImGui::StyleColorsClassic();
#endif

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else
		{
			tictoc.tick();
			calculate_frame_stats();
			update(tictoc.time_interval());
			render(tictoc.time_interval());
		}
	}

#ifdef USE_IMGUI
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif

#if 1
	BOOL fullscreen = 0;
	graphics->get_swap_chain()->GetFullscreenState(&fullscreen, 0);
	if (fullscreen)
	{
		graphics->get_swap_chain()->SetFullscreenState(FALSE, 0);
	}
#endif

	return uninitialize() ? static_cast<int>(msg.wParam) : 0;
}

framework::~framework()
{
}