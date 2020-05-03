#include "main.h"
#include <Windows.h>
#include <stdio.h>
#include <dwmapi.h>
#include "MonitorManager.h"
#include "WindowsManager.h"


LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CREATE:

		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, msg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstace, LPSTR args, int argc)
{
	WNDCLASS wnc;
	HWND hWnd;
	MSG msg;

	wnc.lpfnWndProc = MainWndProc;
	wnc.hInstance = hInstance;
	wnc.hCursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);
	wnc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wnc.lpszMenuName = NULL;
	wnc.cbClsExtra = NULL;
	wnc.hIcon = (HICON)LoadIcon(NULL, IDI_APPLICATION);
	wnc.style = CS_HREDRAW | CS_VREDRAW;
	wnc.cbWndExtra = NULL;
	wnc.lpszClassName = __TEXT("WinTool_Main");
	wnc.lpszMenuName = NULL;
	

	if (!RegisterClass(&wnc))
	{
		MessageBoxA(0, "ERROR1", "ERROR", 0);
		return -1;
	}
	
	hWnd = CreateWindowA(MAINWND_CLASS_NAME, "WinTool",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	
	if (!hWnd)
	{
		MessageBoxA(0, "ERROR2", "ERROR", 0);
		return -2;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;

}


int main()
{

	WindowManager* wm = new WindowManager();
	wm->printWindowList();

	wm->clearWindows();
	Sleep(1000);
	system("cls");
	wm->printWindowList();

	wm->refreshWindowList();
	Sleep(1000);
	system("cls");
	wm->printWindowList();


	return 0;
}
