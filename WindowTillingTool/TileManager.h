#pragma once
#include <list>
#include "WindowsManager.h"
#include "MonitorManager.h"

using namespace std;


#define WINDOW_MARGIN_X 10
#define WINDOW_MARGIN_Y 10


class TileManager {
public:
	TileManager(HWND hMainWnd);
	// ScnÎªScreen µÄËõÐ´
	int getScnCount();
	void refreshScnList();
	CMonitor* getScreen(int index);
	int screenFromWindow(HWND hwnd);
	list<CMonitor>::iterator getScnListItr();
	bool isItrEnd(list<CMonitor>::iterator itr);

	void refreshWinList();
	list<CWindow> getWinListInScn(int imonitor);
	list<CWindow>::iterator getWinListItr();
	bool isItrEnd(list<CWindow>::iterator itr);

	void printWinList();

	void tileWindows();

private:
	HWND mHwndMain;
	WindowsManager* mWinManager;
	MonitorManager* mMonManager;
};


