#include "framework.h"
#include "effekseer_effect_manager.h"
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
	//�G�t�F�N�g�}�l�[�W���[������
	EffekseerEffectManager::Instance().initialize(*graphics);
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
	//�V�[���A�b�v�f�[�g
	SceneManager::instance().update(elapsed_time,*graphics);
}

BOOL framework::get_file_name(HWND hWnd, TCHAR* fname, int sz, TCHAR* initDir)
{
	OPENFILENAMEW o;
	fname[0] = _T('\0');
	ZeroMemory(&o, sizeof(o));
	o.lStructSize = sizeof(o);          // �\���̃T�C�Y
	o.hwndOwner = hWnd;               // �e�E�B���h�E�̃n���h��
	o.lpstrInitialDir = initDir;            // �����t�H���_�[
	o.lpstrFile = fname;              // �擾�����t�@�C������ۑ�����o�b�t�@
	o.nMaxFile = sz;                 // �擾�����t�@�C������ۑ�����o�b�t�@�T�C�Y
	o.lpstrFilter = _TEXT("hlsl�t�@�C��(*.hlsl)\0*.hlsl\0") _TEXT("�S�Ẵt�@�C��(*.*)\0*.*\0");
	o.lpstrDefExt = _TEXT("hlsl");
	o.lpstrTitle = _TEXT("hlsl�t�@�C�����w��");
	o.nFilterIndex = 1;
	return GetOpenFileNameW(&o);
}

void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	//�ʃX���b�h���Ƀf�o�C�X�R���e�L�X�g���g���Ă����ꍇ��
	//�����A�N�Z�X���Ȃ��悤�ɔr�����䂷��
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
	// imgui flags�̏�����
	{
		//Imflags_preset_1 |= ImGuiWindowFlags_NoMove;
	}
	ImGuiIO& io = ImGui::GetIO();
	//----�����Őݒ肵�Ă���ConfigFlags�͑S�̂ɓK�������----//
	// imgui �h�b�L���O�̃t���O    // Enable docking(available in imgui `docking` branch at the moment)
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// ���C���E�B���h�E�̊O�ɏo��  // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	io.ConfigViewportsNoDecoration = true; // imgui�E�B���h�E�����C���E�B���h�E�̊O�ɏo�����Ƃ��S��ʁA�ŏ������ł���: false
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
	EffekseerEffectManager::Instance().finalize();
}