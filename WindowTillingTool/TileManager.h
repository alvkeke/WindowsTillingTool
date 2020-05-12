#pragma once
#include <list>
#include "WindowsManager.h"
#include "MonitorManager.h"

using namespace std;

#define BUFFER_SIZE 100

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

	bool refreshWinList();
	list<CWindow> getWinListInScn(int imonitor);
	list<CWindow>::iterator getWinListItr();
	bool isItrEnd(list<CWindow>::iterator itr);

	void printWinList();

	void addClassBlock(string classname);
	void addTextBlock(string text);
	void addBothBlock(string classname, string text);
	void addHwndBlock(HWND hwnd);

	void clearAllBlock();

	bool checkBlock(CWINITR itr);

	void tileWindows();

private:
	HWND mHwndMain;
	WindowsManager* mWinManager;
	MonitorManager* mMonManager;

	list<string> mTextBlockList;
	list<string> mClassBlockList;
	list<string> mBothBlockList_Text;
	list<string> mBothBlockList_Class;
	list<HWND>	 mHwndBlockList;
};


