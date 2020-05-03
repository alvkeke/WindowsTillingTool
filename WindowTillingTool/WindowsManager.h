#pragma once
#include <iostream>
#include <Windows.h>
#include <dwmapi.h>

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
	int mMonitorIndex;
	HWND mHwnd;
};

class WindowManager {
public:
	WindowManager();
	void refreshWindowList();
	int getWindowCount();
	HWND getWindow(int index);
	void addWindowNode(HWND hwnd);
	void clearWindows();

	void printWindowList();

private:
	WindowNode* mWindowList;
	WindowNode* mWindowTail;

	static BOOL CALLBACK EnumWndProc(HWND hwnd, LPARAM lparam);
};
