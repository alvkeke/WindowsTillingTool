#pragma once
#include <Windows.h>
#include <iostream>

using namespace std;

#define MAINWND_CLASS_NAME "WinTool_Main"

// 界面尺寸
#define COMPOENT_MARGIN 20

#define HEIGTH_MONITORLIST 500
#define WIDTH_MONITORLIST 200

#define HEIGTH_WINDOWLIST 400
#define WIDTH_WINDOWLIST 200

#define HEIGTH_EDITBOX 25
#define WIDTH_EDITBOX 300

// 子控件菜单项ID，用于在事件中判断响应事件的控件
#define ID_CB_MONITOR	1001
#define ID_LB_WINDOWS	2001
#define ID_EB_TEXT		3001
#define ID_EB_CLASS		3002
#define ID_EB_HWND		3003
#define ID_EB_POS		3004
#define ID_EB_SIZE		3005

#define BUFFER_SIZE 100
#define TIMER_INTERVAL_MS 100

void initCompoents(HWND hParent);
void ReadjustMainWindow(HWND hWnd, int nWidth, int nHeight);

void updateWindowInfo();
void updateWinListBox();

void TileWindows();

void lbAddItem(HWND hwnd, char* str);
void lbClearList(HWND hwnd);
void cbAddItem(HWND hwnd, char* str);
void cbClearList(HWND hwnd);


