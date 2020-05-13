#pragma once
#include <Windows.h>

#define MSG_TRAYICON WM_USER


void initTrayIcon(HINSTANCE hInstance, HWND hwnd);
void delTrayIcon();
