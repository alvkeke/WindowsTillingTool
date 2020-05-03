#pragma once
#include <Windows.h>

class MonitorNode {
public:
	MonitorNode* mNext;

	MonitorNode(int primaryflag, int top, int left, int bottom, int right)
	{
		this->miBottom = bottom;
		this->miLeft = left;
		this->miRight = right;
		this->miTop = top;			
		mbPrimary = primaryflag == 1 ? true : false;

		this->mNext = NULL;
	}

	~MonitorNode()
	{
		if (this->mNext)
		{
			delete this->mNext;
		}
	}

	int getTop() { return miTop; }
	int getBottom() { return miBottom; }
	int getLeft() { return miLeft; }
	int getRight() { return miRight; }
	bool isPrimary() { return mbPrimary; }

private:
	bool mbPrimary;
	int miTop;
	int miLeft;
	int miBottom;
	int miRight;
};

class MonitorManager
{
public:
	MonitorManager();
	void refreshMonitors();
	int getMonitorCount();
	MonitorNode* getMonitor(int index);
	void addMonitorNode(int primaryflag, int top, int left, int bottom, int right);
	void clearMonitors();

	int getWidth(int index);
	int getHeight(int index);

	int monitorFromPoint(int x, int y);
	int monitorFromWindow(HWND hwnd);
	
	int getUpMonitor(int index);
	int getLeftMonitor(int index);
	int getRightMonitor(int index);
	int getDownMonitor(int index);

private:

	MonitorNode* mMonitorList;
	MonitorNode* mMonitorTail;

	static BOOL CALLBACK EnumMonitorCallback(HMONITOR handle, HDC hdc, LPRECT rect, LPARAM param);

};
