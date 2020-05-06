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


// #define _CRT_SECURE_NO_WARNINGS

void initCompoents(HWND hParent);
void ReadjustWindow(HWND hWnd, int nWidth, int nHeight);

void lbAddItem(HWND hwnd, char* str);
void lbClearList(HWND hwnd);
void cbAddItem(HWND hwnd, char* str);
void cbClearList(HWND hwnd);



/*

HWND hCBMonitors;
HWND hLBWindows;
HWND hETText;
HWND hETClass;
HWND hETHwnd;
HWND hETPos;
HWND hETSize;
*/

#define ID_CB_MONITOR	1001
#define ID_LB_WINDOWS	2001
#define ID_EB_TEXT		3001
#define ID_EB_CLASS		3002
#define ID_EB_HWND		3003
#define ID_EB_POS		3004
#define ID_EB_SIZE		3005

