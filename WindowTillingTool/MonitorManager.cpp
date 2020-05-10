#include "MonitorManager.h"

BOOL CALLBACK MonitorManager :: EnumMonitorCallback(HMONITOR handle, HDC hdc, LPRECT rect, LPARAM param)
{
	MONITORINFO mi;
	MonitorManager* manager = (MonitorManager*)param;

	mi.cbSize = sizeof(mi);
	GetMonitorInfo(handle, &mi);

	manager->addMonitorNode(mi.dwFlags,
		mi.rcMonitor.top, mi.rcMonitor.left, mi.rcMonitor.bottom, mi.rcMonitor.right);

	return true;
}

MonitorManager :: MonitorManager()
{
	// 初始化变量
	mMonitors.clear();
	// 列举显示器, 并将其存放在screenlist中.
	EnumDisplayMonitors(NULL, NULL, EnumMonitorCallback, (LPARAM)this);
}

void MonitorManager::refreshMonitors()
{

	if (!mMonitors.empty()) mMonitors.clear();
	EnumDisplayMonitors(NULL, NULL, EnumMonitorCallback, (LPARAM)this);
}

int MonitorManager::getMonitorCount()
{
	return mMonitors.size();
}

CMonitor* MonitorManager::getMonitor(int index)
{

	list<CMonitor>::iterator itr = mMonitors.begin();
	if (mMonitors.size() < index) return nullptr;
	for (; index > 0; index--)
	{
		itr++;
	}
	return &(*itr);
}

void MonitorManager::addMonitorNode(int primaryflag, int top, int left, int bottom, int right)
{

	mMonitors.push_back(*new CMonitor(primaryflag, top, left, bottom, right));
}

void MonitorManager::clearMonitors()
{
	mMonitors.clear();
}

int MonitorManager::getWidth(int index)
{
	CMonitor* monitor = getMonitor(index);
	return monitor->getRight() - monitor->getLeft();
}

int MonitorManager::getHeight(int index)
{
	CMonitor* monitor = getMonitor(index);
	return monitor->getBottom() - monitor->getTop();
}

int MonitorManager::getClientWidth(int index)
{
	CMonitor* monitor = getMonitor(index);
	return monitor->getRight() - monitor->getLeft();
}

int MonitorManager::getClientHeight(int index)
{
	CMonitor* monitor = getMonitor(index);
	return monitor->getBottom() - monitor->getTop();
}

int MonitorManager::monitorFromPoint(int x, int y)
{

	list<CMonitor>::iterator itr;
	int i = 0;
	for (itr = mMonitors.begin(); itr != mMonitors.end(); itr++)
	{
		if (itr->getLeft() <= x && itr->getRight() > x &&
			itr->getTop() <= y && itr->getBottom() > y)
		{
			return i;
		}
		i++;
	}
	return -1;
}

int MonitorManager::monitorFromWindow(HWND hwnd)
{
	list<CMonitor>::iterator itr;
	int i = 0;
	RECT r;
	GetWindowRect(hwnd, &r);

	if (IsIconic(hwnd)) return -1;

	if (IsZoomed(hwnd))
	{
		r.left += 7;
		r.top += 7;
	}

	for(itr = mMonitors.begin(); itr!=mMonitors.end(); itr++)
	{
		if (itr->getLeft() <= r.left && itr->getRight() > r.left &&
			itr->getTop() <= r.top && itr->getBottom() > r.top)
		{
			return i;
		}
		i++;
	}

	return -1;
}

int MonitorManager::getUpMonitor(int index)
{
	int i = 0;
	list<CMonitor>::iterator itr;
	CMonitor* m = getMonitor(index);

	for (itr = mMonitors.begin(); itr != mMonitors.end(); itr++)
	{
		if (m->getTop() == itr->getBottom())
		{
			return i;
		}
		i++;
	}

	return -1;
}

int MonitorManager::getLeftMonitor(int index)
{
	int i = 0;
	list<CMonitor>::iterator itr;
	CMonitor* m = getMonitor(index);

	for (itr = mMonitors.begin(); itr != mMonitors.end(); itr++)
	{
		if (m->getLeft() == itr->getRight())
		{
			return i;
		}
		i++;
	}

	return -1;
}

int MonitorManager::getRightMonitor(int index)
{
	int i = 0;
	list<CMonitor>::iterator itr;
	CMonitor* m = getMonitor(index);

	for (itr = mMonitors.begin(); itr != mMonitors.end(); itr++)
	{
		if (m->getRight() == itr->getLeft())
		{
			return i;
		}
		i++;
	}

	return -1;
}

int MonitorManager::getDownMonitor(int index)
{
	int i = 0;
	list<CMonitor>::iterator itr;
	CMonitor* m = getMonitor(index);

	for (itr = mMonitors.begin(); itr != mMonitors.end(); itr++)
	{
		if (m->getBottom() == itr->getTop())
		{
			return i;
		}
		i++;
	}

	return -1;
}
