#pragma once
#include <Windows.h>
#include <iostream>
#include "MonitorManager.h"
#include "WindowsManager.h"

using namespace std;

#define MAINWND_CLASS_NAME "WinTool_Main"
#define COMPOENT_MARGIN 20

#define HEIGTH_MONITORLIST 500
#define WIDTH_MONITORLIST 200

#define HEIGTH_WINDOWLIST 400
#define WIDTH_WINDOWLIST 200

#define HEIGTH_EDITBOX 25
#define WIDTH_EDITBOX 300

void initCompoents(HWND hParent);
void ReadjustWindow(HWND hWnd, int nWidth, int nHeight);


