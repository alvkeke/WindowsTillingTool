#pragma once
#include <Windows.h>
#include <list>

using namespace std;

class CMonitor {
public:

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
};

class MonitorManager
{
public:
	MonitorManager();

	void refreshMonitors();
	int getMonitorCount();
	CMonitor* getMonitor(int index);
	void addMonitorNode(HMONITOR h);
	void clearMonitors();

	list<CMonitor>::iterator getItrBegin();
	bool isItrEnd(list<CMonitor>::iterator itr);

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
