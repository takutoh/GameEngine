#include "App.h"

void Application::Run(HINSTANCE hInstance)
{
	// ウィンドウのクラス名
	LPCWSTR wClassName = L"MyClass";

	// 同じウィンドウがある場合
	if (FindWindow(wClassName, nullptr) != NULL) {
		int id = MessageBox(NULL, L"The game engine is already running.", L"ERROR", MB_ICONERROR | MB_OK);
		return;
	}

	// ウィンドウクラスの設定
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = wClassName;
	RegisterClassEx(&wc);

	// ウィンドウサイズの設定
	RECT rect = {};
	rect.right = static_cast<LONG>(WINDOW_WIDTH);
	rect.bottom = static_cast<LONG>(WINDOW_HEIGHT);

	// ウィンドウサイズの調整
	auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	AdjustWindowRect(&rect, style, FALSE);

	// ウィンドウの生成
	HWND hwnd = CreateWindowEx(
		0,
		wClassName,
		L"GameEngine",
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	// ウィンドウの表示
	ShowWindow(hwnd, SW_SHOWNORMAL);

	// ウィンドウにフォーカス
	SetFocus(hwnd);

	MainLoop();
}

void MainLoop()
{
	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 描画処理
		}
	}
}

LRESULT CALLBACK Application::WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, message, wparam, lparam);
}
