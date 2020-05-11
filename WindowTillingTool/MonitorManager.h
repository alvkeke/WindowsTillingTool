#pragma once
#include <Windows.h>
#include <list>

using namespace std;

class CMonitor {
public:

	//CMonitor(int primaryflag, int top, int left, int bottom, int right);
	CMonitor(HMONITOR hmonitor);

	HMONITOR getHandle();
	int getClientTop();
	int getClientBottom();
	int getClientLeft();
	int getClientRight();
	bool isPrimary();
	void getInfo(LPMONITORINFO mi);
	float getOverlapRate(HWND hwnd);

private:
	HMONITOR hMonitor;
	//bool mbPrimary;
	//int mTop;
	//int mLeft;
	//int mBottom;
	//int mRight;
};

class MonitorManager
{
public:
	MonitorManager();

	void refreshMonitors();
	int getMonitorCount();
	CMonitor* getMonitor(int index);
	// void addMonitorNode(int primaryflag, int top, int left, int bottom, int right);
	void addMonitorNode(HMONITOR h);
	void clearMonitors();

	int getWidth(int index);
	int getHeight(int index);

	int getClientWidth(int index);
	int getClientHeight(int index);

	int monitorFromPoint(int x, int y);
	int monitorFromWindow(HWND hwnd);
	
	int getUpMonitor(int index);
	int getLeftMonitor(int index);
	int getRightMonitor(int index);
	int getDownMonitor(int index);

private:

	list<CMonitor> mMonitors;

	static BOOL CALLBACK EnumMonitorCallback(HMONITOR handle, HDC hdc, LPRECT rect, LPARAM param);

};
