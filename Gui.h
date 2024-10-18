#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Gui
{
public:
	bool Init(HWND hwnd); // ‰Šú‰»
	void Update();        // XVˆ—
	void Draw();          // •`‰æˆ—
	void End();           // I—¹ˆ—

	bool isPlaying = false;

private:
	HWND hwnd;
};

extern Gui* g_Gui;
