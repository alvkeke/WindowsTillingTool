#pragma once
#include <iostream>
#include <Windows.h>
#include <dwmapi.h>
#include "MonitorManager.h"

using namespace std;

#pragma comment(lib, "dwmapi.lib")

#define IGNORE_WND_CLASS_MAX_LEN 30

class WindowNode {
public:
	WindowNode* mNext;

	WindowNode(HWND hwnd);
	~WindowNode();
	HWND getHandle();
	void getPos(LPPOINT p);
	void setPos(int x, int y);
	void getRect(LPRECT rect);
	void setSize(int w, int h);
	void setRect(LPRECT rect);

	HWND getHwnd();
	void getText(char* buf, int n_buf);
	void getClassName(char* buf, int n_buf);

private:
	HWND mHwnd;
};

class WindowListHead {
public :
	WindowListHead(int iscreen);
	~WindowListHead();

	WindowListHead* nextScreen;
	WindowNode* mWindowList;
	WindowNode* mWindowTail;
	int MonitorIndex;
};

class WindowManager {
public:
	WindowManager();

	void refreshWindowList();
	int getAllWindowCount();
	WindowNode* getWindowNode(int screen, int index);
	HWND getWindow(int screen, int index);
	void addWindowNode(HWND hwnd);
	void clearWindows();

	void printWindowList();
	MonitorManager* getMonitormanager();
private:
	MonitorManager* mMonitors;
	WindowListHead* mAllWindows;

	static BOOL CALLBACK EnumWndProc(HWND hwnd, LPARAM lparam);
};
