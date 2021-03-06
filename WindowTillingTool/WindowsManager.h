#pragma once
#include <iostream>
#include <list>
#include <Windows.h>
#include <dwmapi.h>
#include "MonitorManager.h"

using namespace std;

#pragma comment(lib, "dwmapi.lib")

#define IGNORE_WND_CLASS_MAX_LEN 256


class CWindow {
public:

	CWindow(HWND hwnd);

	HWND getHandle();
	void getPos(LPPOINT p);
	void setPos(int x, int y);

	void setSize(int w, int h);

	void getRect(LPRECT rect);
	void setRect(LPRECT rect);

	void getText(char* buf, int n_buf);
	void getClassName(char* buf, int n_buf);

	boolean isNormalShow();

private:
	HWND mHwnd;
};

typedef list<CWindow>::iterator CWINITR;

class WindowsManager {
public:
	WindowsManager();

	bool refreshWindowList();
	void addWindowNode(HWND hwnd);
	void clearWindows();
	void printWindowList();
	void setListChanged();

	int getAllWindowCount();
	int getAllShowWindowCount();

	CWindow* getWindow(int index);
	HWND getHwnd(int index);
	list<CWindow>* getWindowList();
	list<CWindow>::iterator getItrBegin();
	bool isItrEnd(list<CWindow>::iterator);
	bool isWinInList(HWND hwnd);

	void clearOutdateWindows();

private:
	list<CWindow> mAllWindows;
	bool bListChanged;
	static BOOL CALLBACK EnumWndProc(HWND hwnd, LPARAM lparam);
};
