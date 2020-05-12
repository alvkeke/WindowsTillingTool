#include "TrayIcon.h"

NOTIFYICONDATA trayicon;

void initTrayIcon(HWND hwnd)
{
	trayicon.cbSize = sizeof(NOTIFYICONDATA);
	trayicon.hWnd = hwnd;
	trayicon.uID = 0;
	trayicon.dwInfoFlags = NULL;
	trayicon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	trayicon.uCallbackMessage = MSG_TRAYICON;
	trayicon.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	strcpy(trayicon.szTip, "test");
	Shell_NotifyIcon(NIM_ADD, &trayicon);
}

void delTrayIcon()
{
	Shell_NotifyIcon(NIM_DELETE, &trayicon);
}
