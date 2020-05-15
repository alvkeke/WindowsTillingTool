#pragma once
#include <list>
#include "WindowsManager.h"
#include "MonitorManager.h"

using namespace std;

#define BUFFER_SIZE 256

#define WINDOW_PADDING_X 10
#define WINDOW_PADDING_Y 10

class TileWinInfo {

public:
	TileWinInfo(HWND hwnd);
	HWND getHandle();
	void updateData();
	void setWidth(int w);
	void setHeight(int h);
	bool isDataChanged();
	bool isDataDifferent(bool zoomState, int x, int y, int w, int h);
	bool isUserSetMaxed();
	void usersetMaxed(bool foo);

	//void indexChanged();

	void move(int, int, int, int);

private:
	HWND mhWnd;
	//bool mPosChanged;
	bool mIsUserSetMaxed;
	int mOldX, mOldY, mOldW, mOldH;
	bool mOldZoomedState;
	int mSetWid, mSetHei;
};

typedef list<TileWinInfo>::iterator TILEWINITR;

class TileScnInfo {

public:
	TileScnInfo(HMONITOR hscn);

	HMONITOR getHandle();
	void addWin(HWND hwnd);
	void delWin(HWND hwnd);
	TILEWINITR delWin(TILEWINITR itr);
	int indexWin(HWND hwnd);
	int getWinCount();
	void clearWins();

	void stepWinLeft(HWND hwnd);
	void stepWinRight(HWND hwnd);
	void setFullWin(HWND hwnd);
	void cancelFullWin();

	TILEWINITR getWinInfoItr();
	bool isItrEnd(TILEWINITR itr);

private:
	int mFullWin;
	list<TileWinInfo> mWins;
	HMONITOR mhScn;
};

typedef list<TileScnInfo>::iterator TILESCNITR;

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
	bool isWinInList(HWND hwnd);

	void printWinList();

	void addClassBlock(string classname);
	void addTextBlock(string text);
	void addBothBlock(string classname, string text);
	void addHwndBlock(HWND hwnd);
	void addClassPartBlock(string classname);
	void addTextPartBlock(string text);

	void clearAllBlock();

	bool checkBlock(CWINITR itr);

	void moveFocusWindowLeft();
	void moveFocusWindowRight();

	void setTilePadding(int x/*-1 no change*/, int y/*-1 no change*/);
	TileScnInfo* getScnInfo(int iScn);
	TileScnInfo* getScnInfo(HMONITOR handle);
	void addWinInfo(int iScn, HWND hwnd);
	void assignWinInfoList();
	void tileWindows();
	//void tileWindows2();

private:
	HWND mHwndMain;

	int mPaddingX, mPaddingY;

	WindowsManager* mWinManager;
	MonitorManager* mMonManager;

	list<TileScnInfo> mScnInfo;

	list<string> mTextBlockList;
	list<string> mClassBlockList;
	list<string> mTextPartBlockList;
	list<string> mClassPartBlockList;
	list<string> mBothBlockList_Text;
	list<string> mBothBlockList_Class;
	list<HWND>	 mHwndBlockList;
};


