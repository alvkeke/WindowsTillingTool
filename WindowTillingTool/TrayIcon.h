#pragma once
#include <Windows.h>

#define MSG_TRAYICON WM_USER


void initTrayIcon(HWND hwnd);
void delTrayIcon();
