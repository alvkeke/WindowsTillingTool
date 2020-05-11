#include "MonitorManager.h"


CMonitor::CMonitor(int primaryflag, int top, int left, int bottom, int right)
{
	this->miBottom = bottom;
	this->miLeft = left;
	this->miRight = right;
	this->miTop = top;
	mbPrimary = primaryflag == 1 ? true : false;
}

int CMonitor::getTop() 
{ 
	return miTop; 
}
int CMonitor::getBottom() 
{ 
	return miBottom; 
}
int CMonitor::getLeft() 
{ 
	return miLeft; 
}
int CMonitor::getRight() 
{ 
	return miRight; 
}
bool CMonitor::isPrimary() 
{ 
	return mbPrimary; 
}

float CMonitor::getOverlapRate(HWND hwnd)
{
	RECT r;
	float overlapsize;
	float winsize;
	int ah, aw, h1, h2, h3, w1, w2;

	GetWindowRect(hwnd, &r);

	if (r.left >= miRight || r.right < miLeft || r.bottom < miTop || r.top >= miBottom) return 0;
	ah = max(r.bottom, miBottom) - min(r.top, miTop);
	aw = max(r.right, miRight) - min(r.left, miLeft);
	h1 = max(r.top, miTop) - min(r.top, miTop);
	h2 = max(r.bottom, miBottom) - min(r.bottom, miBottom);
	h3 = ah - h1 - h2;
	w1 = max(r.left, miLeft) - min(r.left, miLeft);
	w2 = max(r.right, miRight) - min(r.right, miRight);

	overlapsize = aw * ah - aw*h1 - aw*h2 - w2*h3 - w1*h3;
	winsize = (r.right - r.left) * (r.bottom - r.top);

	return overlapsize/winsize;
}


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
	int imonitor = 0;
	int savemonitor = 0;
	float saveoverlaprate = 0;

	for(itr = mMonitors.begin(); itr!=mMonitors.end(); itr++)
	{

		float t = itr->getOverlapRate(hwnd);
		if (t > saveoverlaprate)
		{
			saveoverlaprate = t;
			savemonitor = imonitor;
		}
		imonitor++;
	}

	if (saveoverlaprate == 0) 
		return -1; //与所有窗口都没有交集

	return savemonitor;
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
