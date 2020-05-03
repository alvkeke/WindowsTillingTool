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
	mMonitorList = NULL;
	mMonitorTail = NULL;
	// 列举显示器, 并将其存放在screenlist中.
	EnumDisplayMonitors(NULL, NULL, EnumMonitorCallback, (LPARAM)this);
}

void MonitorManager::refreshMonitors()
{
	if (mMonitorList) delete mMonitorList;
	mMonitorList = NULL;
	mMonitorTail = NULL;
	EnumDisplayMonitors(NULL, NULL, EnumMonitorCallback, (LPARAM)this);
}

int MonitorManager::getMonitorCount()
{
	int n = 0;
	for (MonitorNode* itr = mMonitorList; itr != NULL; itr = itr->mNext)
		n++;
	return n;
}

MonitorNode* MonitorManager::getMonitor(int index)
{
	MonitorNode* itr = mMonitorList;
	for (; index > 0; index--)
	{
		if (itr->mNext == NULL) return nullptr;
		itr = itr->mNext;
	}
	return itr;
}

void MonitorManager::addMonitorNode(int primaryflag, int top, int left, int bottom, int right)
{
	if (mMonitorList == NULL)
	{
		mMonitorList = new MonitorNode(primaryflag, top, left, bottom, right);
		mMonitorTail = mMonitorList;
	}
	else
	{
		mMonitorTail->mNext = new MonitorNode(primaryflag, top, left, bottom, right);
		mMonitorTail = mMonitorTail->mNext;
	}
}

void MonitorManager::clearMonitors()
{
	delete mMonitorList;
	mMonitorList = NULL;
	mMonitorTail = NULL;
}

int MonitorManager::getWidth(int index)
{
	MonitorNode* monitor = getMonitor(index);
	return monitor->getRight() - monitor->getLeft();
}

int MonitorManager::getHeight(int index)
{
	MonitorNode* monitor = getMonitor(index);
	return monitor->getBottom() - monitor->getTop();
}

int MonitorManager::monitorFromPoint(int x, int y)
{
	int i = 0;

	for (MonitorNode* itr = mMonitorList; itr != NULL; itr = itr->mNext)
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
	int i = 0;
	RECT r;
	GetWindowRect(hwnd, &r);

	if (IsIconic(hwnd)) return -1;

	if (IsZoomed(hwnd))
	{
		r.left += 7;
		r.top += 7;
	}

	for (MonitorNode* itr = mMonitorList; itr != NULL; itr = itr->mNext)
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
	MonitorNode * m = getMonitor(index);

	for (MonitorNode* itr = mMonitorList; itr != NULL; itr = itr->mNext)
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
	MonitorNode* m = getMonitor(index);

	for (MonitorNode* itr = mMonitorList; itr != NULL; itr = itr->mNext)
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
	MonitorNode* m = getMonitor(index);

	for (MonitorNode* itr = mMonitorList; itr != NULL; itr = itr->mNext)
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
	MonitorNode* m = getMonitor(index);

	for (MonitorNode* itr = mMonitorList; itr != NULL; itr = itr->mNext)
	{
		if (m->getBottom() == itr->getTop())
		{
			return i;
		}
		i++;
	}

	return -1;
}
