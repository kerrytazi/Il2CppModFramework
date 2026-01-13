#include "pch.hpp"

#include "ImGuiHook.hpp"

#include "common/Log.hpp"
#include "common/StringUtils.hpp"
#include "UnityEngine/SystemInfo.hpp"

#include "static_lambda/detour_lambda.hpp"

#include "ImDrawDataCopy.hpp"

#include "imgui.h"

#pragma comment(linker, "/alternatename:GetImGuiIniFilename=DefaultGetImGuiIniFilename")
extern "C" const char* GetImGuiIniFilename();
extern "C" const char* DefaultGetImGuiIniFilename()
{
	return "config/imgui.ini";
}

#pragma comment(linker, "/alternatename:GetImGuiLogFilename=DefaultGetImGuiLogFilename")
extern "C" const char* GetImGuiLogFilename();
extern "C" const char* DefaultGetImGuiLogFilename()
{
	return "client/imgui.log";
}

extern const std::string& GetExeDir();

static std::string g_ini_path;
static std::string g_log_path;

static const std::string& GetIniPath()
{
	if (g_ini_path.empty() && GetImGuiIniFilename() != nullptr)
		g_ini_path = GetExeDir() + "/" + GetImGuiIniFilename();

	return g_ini_path;
}

static const std::string& GetLogPath()
{
	if (g_ini_path.empty() && GetImGuiLogFilename() != nullptr)
		g_ini_path = GetExeDir() + "/" + GetImGuiLogFilename();

	return g_log_path;
}

static void ImGuiInitBasic()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

	if (!GetLogPath().empty())
		io.LogFilename = GetLogPath().c_str();

	if (!GetIniPath().empty())
		ImGui::LoadIniSettingsFromDisk(GetIniPath().c_str());

	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
}


#ifdef _WIN32

#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include <d3d11.h>
#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")

extern ImDrawDataCopy g_imgui_draw_data_renderer_copy;

struct WindowData
{
	DWORD targetProcessId;
	HWND foundWindow;
};

static bool IsConsoleWindow(HWND hwnd)
{
	const int MAX_CLASS_NAME = 256;
	WCHAR className[MAX_CLASS_NAME];

	if (GetClassNameW(hwnd, className, MAX_CLASS_NAME) > 0)
		return wcscmp(className, L"ConsoleWindowClass") == 0;

	return false;
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	auto data = reinterpret_cast<WindowData*>(lParam);

	DWORD processId = 0;
	GetWindowThreadProcessId(hwnd, &processId);

	if (processId == data->targetProcessId)
	{
		if (IsWindowVisible(hwnd) && GetWindowTextLength(hwnd) > 0 && !IsConsoleWindow(hwnd))
		{
			data->foundWindow = hwnd;
			return FALSE;
		}
	}

	return TRUE;
}

static HWND FindFirstWindowForProcess(DWORD processId)
{
	WindowData data;
	data.targetProcessId = processId;
	data.foundWindow = NULL;

	EnumWindows(&EnumWindowsProc, reinterpret_cast<LPARAM>(&data));

	return data.foundWindow;
}

#pragma comment(linker, "/alternatename:GetUnityGameWindow=DefaultGetUnityGameWindow")
extern "C" HWND GetUnityGameWindow();
extern "C" HWND DefaultGetUnityGameWindow()
{
	return FindFirstWindowForProcess(GetCurrentProcessId());
}

#pragma comment(linker, "/alternatename:GetImGuiSwitchClientMenuKey=DefaultGetImGuiSwitchClientMenuKey")
extern "C" int GetImGuiSwitchClientMenuKey();
extern "C" int DefaultGetImGuiSwitchClientMenuKey()
{
	return VK_INSERT;
}

static bool ShouldBlockWndEvent(UINT msg)
{
	switch (msg)
	{
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
		case WM_MOUSELEAVE:
		case WM_NCMOUSELEAVE:
		case WM_INPUT:

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONDBLCLK:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
			return true;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_SETFOCUS:
		case WM_KILLFOCUS:
		case WM_INPUTLANGCHANGE:
		case WM_CHAR:
		case WM_SETCURSOR:
			return true;
	}

	return false;
}


static std::atomic<bool> g_render_active = false;
static bool g_client_menu_active = false;
static RECT g_last_set_clip_rect;
static std::optional<RECT*> g_last_set_clip_rect_ptr;
static std::optional<POINT> g_last_set_cursor_pos;
static POINT g_last_get_cursor_pos;


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

static WNDPROC g_original_wnd_proc = nullptr;

static LRESULT APIENTRY PatchHwndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	assert(g_original_wnd_proc);

	if ((msg == WM_KEYDOWN || msg == WM_KEYUP) && wparam == GetImGuiSwitchClientMenuKey())
	{
		if (msg == WM_KEYDOWN && (lparam & 0x40000000) == 0)
			g_client_menu_active = !g_client_menu_active;

		return 1;
	}

	if (g_render_active && ImGui::GetCurrentContext())
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
			return DefWindowProcW(hwnd, msg, wparam, lparam);

		if (ShouldBlockWndEvent(msg))
			return 1;
	}

	return CallWindowProcW(g_original_wnd_proc, hwnd, msg, wparam, lparam);
}


static HWND g_hwnd = nullptr;
static std::atomic<bool> g_imgui_initialized = false;

static std::unique_ptr<sl::detour<HRESULT __stdcall (IDXGISwapChain*, UINT, UINT)>> g_d3d11_present_detour;
static std::unique_ptr<sl::detour<BOOL WINAPI(const RECT*)>> g_clip_cursor_detour;
static std::unique_ptr<sl::detour<BOOL WINAPI(int, int)>> g_set_cursor_pos_detour;
static std::unique_ptr<sl::detour<BOOL WINAPI(LPPOINT)>> g_get_cursor_pos_detour;

static bool LoadD3D11()
{
	Log::Debug("LoadD3D11()");

	g_hwnd = GetUnityGameWindow();

	Log::Debug("g_hwnd: " + std::format("{:#016x}", (uint64_t)g_hwnd));

	if (!g_hwnd)
	{
		Log::Error("Can't find window");
		return false;
	}

	ImGuiInitBasic();

#ifdef _WIN32
	ImGui_ImplWin32_EnableDpiAwareness();
	float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

	ImGui_ImplWin32_Init(g_hwnd);
#endif // _WIN32

	void** pSwapChainVTable = nullptr;

	{
		HINSTANCE hinstance = GetModuleHandle(NULL);
		WNDCLASSEXW wc{};
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = &DefWindowProcW;
		wc.hInstance = hinstance;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.lpszClassName = L"ImGuiHook_Class";
		RegisterClassExW(&wc);

		HWND hwnd = CreateWindowExW(0, L"ImGuiHook_Class", L"ImGuiHook",
							WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
							1, 1, NULL, NULL, hinstance, NULL);

		D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
		D3D_FEATURE_LEVEL obtainedLevel;
		DXGI_SWAP_CHAIN_DESC sd{};

		sd.BufferCount = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.OutputWindow = hwnd;
		sd.SampleDesc.Count = 1;
		sd.Windowed = true;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		sd.BufferDesc.Width = 1;
		sd.BufferDesc.Height = 1;
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 1;

		ID3D11Device* pd3dDevice = nullptr;
		ID3D11DeviceContext* pd3dContext = nullptr;
		IDXGISwapChain* pSwapChain = nullptr;

		if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, levels, sizeof(levels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, &obtainedLevel, &pd3dContext)))
		{
			Log::Error("Failed to create device and swapchain");
			return false;
		}

		pSwapChainVTable = ((void***)pSwapChain)[0];

		pd3dDevice->Release();
		pd3dContext->Release();
		pSwapChain->Release();

		DestroyWindow(hwnd);
	}

	g_clip_cursor_detour = std::make_unique<sl::detour<BOOL WINAPI(const RECT*)>>(&ClipCursor, [](const RECT *lpRect, auto original) -> BOOL {
		if (g_render_active)
		{
			if (lpRect)
			{
				g_last_set_clip_rect = *lpRect;
				g_last_set_clip_rect_ptr = &g_last_set_clip_rect;
			}
			else
			{
				g_last_set_clip_rect_ptr = nullptr;
			}

			return TRUE;
		}

		return original(lpRect);
	});

	g_set_cursor_pos_detour = std::make_unique<sl::detour<BOOL WINAPI(int, int)>>(&SetCursorPos, [](int X, int Y, auto original) -> BOOL {
		if (g_render_active)
		{
			g_last_set_cursor_pos = POINT{ X, Y };
			return TRUE;
		}

		return original(X, Y);
	});

	g_get_cursor_pos_detour = std::make_unique<sl::detour<BOOL WINAPI(LPPOINT)>>(&GetCursorPos, [](LPPOINT lpPoint, auto original) -> BOOL {
		if (g_render_active)
		{
			*lpPoint = g_last_get_cursor_pos;
			return TRUE;
		}

		return original(lpPoint);
	});

	g_original_wnd_proc = (WNDPROC)SetWindowLongPtrW(g_hwnd, GWLP_WNDPROC, (LONG_PTR)&PatchHwndProc);

	g_d3d11_present_detour = std::make_unique<sl::detour<HRESULT __stdcall (IDXGISwapChain*, UINT, UINT)>>(pSwapChainVTable[8], [](IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags, auto original) {
		if (g_render_active || !g_imgui_initialized)
		{
			ID3D11Device* pd3dDevice = nullptr;
			ID3D11DeviceContext* pd3dContext = nullptr;

			pSwapChain->GetDevice(__uuidof(pd3dDevice), (void**)&pd3dDevice);
			pd3dDevice->GetImmediateContext(&pd3dContext);

			if (!g_imgui_initialized)
			{
				g_imgui_initialized = true;
				ImGui_ImplDX11_Init(pd3dDevice, pd3dContext);
			}

			ID3D11RenderTargetView* pOriginalRTV = nullptr;
			ID3D11DepthStencilView* pOriginalDSV = nullptr;
			pd3dContext->OMGetRenderTargets(1, &pOriginalRTV, &pOriginalDSV);

			ID3D11Texture2D* pBackBuffer = nullptr;
			if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer)))
			{
				ID3D11RenderTargetView* pMainRTV = nullptr;
				if (SUCCEEDED(pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pMainRTV)))
				{
					pd3dContext->OMSetRenderTargets(1, &pMainRTV, nullptr);


					ImGui_ImplDX11_NewFrame();
					ImGui_ImplDX11_RenderDrawData(&*g_imgui_draw_data_renderer_copy.Locked());


					pMainRTV->Release();
				}
				else
				{
					Log::Error("CreateRenderTargetView failed");
				}

				pBackBuffer->Release();
			}
			else
			{
				Log::Error("pSwapChain->GetBuffer 0 failed");
			}

			pd3dContext->OMSetRenderTargets(1, &pOriginalRTV, pOriginalDSV);

			if (pOriginalRTV)
				pOriginalRTV->Release();

			if (pOriginalDSV)
				pOriginalDSV->Release();

			pd3dDevice->Release();
			pd3dContext->Release();
		}

		return original(pSwapChain, SyncInterval, Flags);
	});

	return true;
}

static void UnloadD3D11()
{
	Log::Debug("UnloadD3D11()");

	ImGuiHook::SetRenderActive(false);

	SetWindowLongPtrW(g_hwnd, GWLP_WNDPROC, (LONG_PTR)g_original_wnd_proc);

	g_d3d11_present_detour.reset();
	g_clip_cursor_detour.reset();
	g_set_cursor_pos_detour.reset();
	g_get_cursor_pos_detour.reset();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
}

#endif // _WIN32

bool ImGuiHook::Load()
{
	auto type = UnityEngine::SystemInfo::GetGraphicsDeviceType();

	switch (type)
	{
#ifdef _WIN32
		case UnityEngine::Rendering::GraphicsDeviceType::Direct3D11:
			if (!LoadD3D11())
				return false;
			break;
#endif // _WIN32
		default:
			Log::Error("ImGuiHook::Load(): ImGui is not implemented for: " + su::u8((int)type));
			return false;
	}

	return true;
}

bool ImGuiHook::Unload()
{
	auto type = UnityEngine::SystemInfo::GetGraphicsDeviceType();

	switch (type)
	{
#ifdef _WIN32
		case UnityEngine::Rendering::GraphicsDeviceType::Direct3D11:
			UnloadD3D11();
			break;
#endif // _WIN32
		default:
			Log::Error("ImGuiHook::Unload(): ImGui is not implemented for: " + su::u8((int)type));
			return false;
	}

	if (!GetIniPath().empty())
	{
		std::filesystem::create_directories(std::filesystem::path(GetIniPath()).parent_path());
		ImGui::SaveIniSettingsToDisk(GetIniPath().c_str());
	}

	ImGui::DestroyContext();

	return true;
}

void ImGuiHook::NewFrame()
{
#ifdef _WIN32
	ImGui_ImplWin32_NewFrame();
#endif // _WIN32
}

void ImGuiHook::SetRenderActive(bool active)
{
	bool prev_show = g_render_active;

	if (active == prev_show)
		return;

#ifdef _WIN32
	if (!prev_show)
	{
		if (GetClipCursor(&g_last_set_clip_rect))
			g_last_set_clip_rect_ptr = &g_last_set_clip_rect;

		ClipCursor(nullptr);
		GetCursorPos(&g_last_get_cursor_pos);
	}
#endif // _WIN32

	g_render_active = !prev_show;

#ifdef _WIN32
	if (prev_show)
	{
		if (g_last_set_clip_rect_ptr.has_value())
		{
			ClipCursor(*g_last_set_clip_rect_ptr);
			g_last_set_clip_rect_ptr = std::nullopt;
		}

		if (g_last_set_cursor_pos.has_value())
		{
			SetCursorPos(g_last_set_cursor_pos->x, g_last_set_cursor_pos->y);
			g_last_set_cursor_pos = std::nullopt;
		}
	}

	ShowCursor(prev_show ? FALSE : TRUE);
#endif // _WIN32
}

bool ImGuiHook::IsClientMenuActive()
{
	return g_client_menu_active;
}

bool ImGuiHook::IsFullyInitialized()
{
#ifdef _WIN32
	return g_imgui_initialized;
#endif // _WIN32
}
