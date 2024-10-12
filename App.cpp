#include "App.h"
#include "Engine.h"
#include "Scene.h"

void Application::Run(HINSTANCE hInstance)
{
	// �E�B���h�E�̃N���X��
	LPCWSTR wClassName = L"MyClass";

	// �����E�B���h�E������ꍇ
	if (FindWindow(wClassName, nullptr) != NULL) {
		int id = MessageBox(NULL, L"The game engine is already running.", L"ERROR", MB_ICONERROR | MB_OK);
		return;
	}

	// �E�B���h�E�N���X�̐ݒ�
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

	// �E�B���h�E�T�C�Y�̐ݒ�
	RECT rect = {};
	rect.right = static_cast<LONG>(WINDOW_WIDTH);
	rect.bottom = static_cast<LONG>(WINDOW_HEIGHT);

	// �E�B���h�E�T�C�Y�̒���
	auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	AdjustWindowRect(&rect, style, FALSE);

	// �E�B���h�E�̐���
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

	// �E�B���h�E�̕\��
	ShowWindow(hwnd, SW_SHOWNORMAL);

	// �E�B���h�E�Ƀt�H�[�J�X
	SetFocus(hwnd);

	// �`��G���W���̏�����
	g_Engine = new Engine();
	if (!g_Engine->Init(hwnd, WINDOW_WIDTH, WINDOW_HEIGHT))
	{
		return;
	}

	// �V�[���̏�����
	g_Scene = new Scene();
	if (!g_Scene->Init())
	{
		return;
	}

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
			// �`�揈��
			g_Scene->Update();
			g_Engine->BeginRender();
			g_Scene->Draw();
			g_Engine->EndRender();
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
