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


void initCompoents(HWND hParent);
void updateWindowInfo();
void updateWinListBox();
int lbGetSelectItem();
void lbAddItem(HWND hwnd, char* str);
void lbClearList(HWND hwnd);
void cbAddItem(HWND hwnd, char* str);
void cbClearList(HWND hwnd);
void popupMenu(HWND hwnd);

void enableTiling();
void disableTiling();
