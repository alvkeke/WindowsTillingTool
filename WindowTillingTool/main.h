#pragma once
#include <Windows.h>
#include <iostream>
#include "common.h"

using namespace std;


// 界面尺寸
#define COMPOENT_MARGIN 20

#define HEIGTH_MONITORLIST 500
#define WIDTH_MONITORLIST 200

#define HEIGTH_WINDOWLIST 400
#define WIDTH_WINDOWLIST 200

#define HEIGTH_EDITBOX 25
#define WIDTH_EDITBOX 300

// 子控件菜单项ID，用于在事件中判断响应事件的控件
#define ID_CB_MONITOR	11001
#define ID_LB_WINDOWS	12001
#define ID_EB_TEXT		13001
#define ID_EB_CLASS		13002
#define ID_EB_HWND		13003
#define ID_EB_POS		13004
#define ID_EB_SIZE		13005

#define TIMER_INTERVAL_MS 100
#define PROTECT_INTERVAL_MS 1000

// 键盘鼠标hook的宏定义
#define HOOK_KEY_FUNC VK_LWIN
#define HOOK_KEY_MOUSETOOL_SWITCH VK_F2
#define HOOK_KEY_TILE_SWITCH VK_F1
#define HOOK_KEY_FULLWINDOW 'F'

//#define HOOK_KEY_UP 'I'
//#define HOOK_KEY_DOWN 'K'
//#define HOOK_KEY_LEFT 'J'
//#define HOOK_KEY_RIGHT 'L'

#define HOOK_KEY_UP VK_UP
#define HOOK_KEY_DOWN VK_DOWN
#define HOOK_KEY_LEFT VK_LEFT
#define HOOK_KEY_RIGHT VK_RIGHT


void initCompoents(HWND hParent);
void updateWindowInfo();
void updateWinListBox();
int lbGetSelectItem();
void lbAddItem(HWND hwnd, char* str);
void lbClearList(HWND hwnd);
void cbAddItem(HWND hwnd, char* str);
void cbClearList(HWND hwnd);
void popupMenu(HWND hwnd);

void loadConfiguration();

void tileAddHwndBlock(HWND hwnd);
void tileAddClassBlock(string classname);
void tileAddTextBlock(string text);
void tileAddBothBlock(string classname, string text);
void enableTiling();
void disableTiling(); 
int initHook(HINSTANCE hInstance);
void enableMouseTool();
void disableMouseTool();

void terminateApplication();
