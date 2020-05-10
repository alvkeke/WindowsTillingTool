#pragma once
#include <iostream>
#include <list>
#include <Windows.h>
#include <dwmapi.h>
#include "MonitorManager.h"

using namespace std;

#pragma comment(lib, "dwmapi.lib")

#define IGNORE_WND_CLASS_MAX_LEN 100

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

class WindowsManager {
public:
	WindowsManager();

	// MonitorManager* getMonitormanager();

	void refreshWindowList();
	void addWindowNode(HWND hwnd);
	void clearWindows();
	void printWindowList();

	int getAllWindowCount();
	// int getWindowCount(int screen);
	int getAllShowWindowCount();
	// int getShowWindowCount(int screen);

	/*
	CWindow* getWindowNode(int screen, int index);
	HWND getWindow(int screen, int index);
	CWindow* getWindowList(int screen);
	*/

	CWindow* getWindow(int index);
	HWND getHwnd(int index);
	list<CWindow>* getWindowList();
	list<CWindow>::iterator getItrBegin();
	bool isItrEnd(list<CWindow>::iterator);

	//void moveWindow(int screen, int index, int x, int y, int w, int h);
	

private:
	// MonitorManager* mMonitors;
	// WindowListHead* mAllWindows;
	list<CWindow> mAllWindows;

	static BOOL CALLBACK EnumWndProc(HWND hwnd, LPARAM lparam);
};
