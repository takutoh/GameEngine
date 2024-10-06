#pragma once
#include <Windows.h>

class Application
{
public:
	static void Run(HINSTANCE hInstance);
	static const UINT WINDOW_WIDTH = 1280;
	static const UINT WINDOW_HEIGHT = 720;

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
};

void MainLoop();
