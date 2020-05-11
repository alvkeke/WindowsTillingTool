#include "MonitorManager.h"

/*
CMonitor::CMonitor(int primaryflag, int top, int left, int bottom, int right)
{
	this->miBottom = bottom;
	this->miLeft = left;
	this->miRight = right;
	this->miTop = top;
	mbPrimary = primaryflag == 1 ? true : false;
}
*/
CMonitor::CMonitor(HMONITOR hmonitor)
{
	//MONITORINFO mi;

	hMonitor = hmonitor;
	//mi.cbSize = sizeof(mi);
	//GetMonitorInfo(hMonitor, &mi);

	//mBottom = mi.rcMonitor.bottom;
	//mLeft = mi.rcMonitor.left;
	//mRight = mi.rcMonitor.right;
	//mTop = mi.rcMonitor.top;
	//mbPrimary = mi.dwFlags == 1 ? true : false;
}

HMONITOR CMonitor::getHandle()
{
	return hMonitor;
}

int CMonitor::getClientTop()
{
	MONITORINFO mi;
	getInfo(&mi);
	return mi.rcWork.top; 
}
int CMonitor::getClientBottom() 
{ 
	MONITORINFO mi;
	getInfo(&mi);
	return mi.rcWork.bottom;
}
int CMonitor::getClientLeft() 
{ 
	MONITORINFO mi;
	getInfo(&mi);
	return mi.rcWork.left;
}
int CMonitor::getClientRight() 
{ 
	MONITORINFO mi;
	getInfo(&mi);
	return mi.rcWork.right;
}
bool CMonitor::isPrimary() 
{ 
	MONITORINFO mi;
	getInfo(&mi);
	return mi.dwFlags == 1;
}

void CMonitor::getInfo(LPMONITORINFO mi)
{
	mi->cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(this->hMonitor, mi);
}

float CMonitor::getOverlapRate(HWND hwnd)
{
	RECT r;
	MONITORINFO mi;
	float overlapsize;
	float winsize;
	int ah, aw, h1, h2, h3, w1, w2;
	int mRight, mLeft, mTop, mBottom;

	GetWindowRect(hwnd, &r);
	GetMonitorInfo(hMonitor, &mi);
	mRight = mi.rcMonitor.right;
	mLeft = mi.rcMonitor.left;
	mTop = mi.rcMonitor.top;
	mBottom = mi.rcMonitor.bottom;

	if (r.left >= mRight || r.right < mLeft || r.bottom < mTop || r.top >= mBottom) return 0;

	ah = max(r.bottom, mBottom) - min(r.top, mTop);
	aw = max(r.right, mRight) - min(r.left, mLeft);
	h1 = max(r.top, mTop) - min(r.top, mTop);
	h2 = max(r.bottom, mBottom) - min(r.bottom, mBottom);
	h3 = ah - h1 - h2;
	w1 = max(r.left, mLeft) - min(r.left, mLeft);
	w2 = max(r.right, mRight) - min(r.right, mRight);

	overlapsize = aw * ah - aw*h1 - aw*h2 - w2*h3 - w1*h3;
	winsize = (r.right - r.left) * (r.bottom - r.top);

	return overlapsize/winsize;
}


BOOL CALLBACK MonitorManager :: EnumMonitorCallback(HMONITOR handle, HDC hdc, LPRECT rect, LPARAM param)
{
	//MONITORINFO mi;
	MonitorManager* manager = (MonitorManager*)param;

	//mi.cbSize = sizeof(mi);
	//GetMonitorInfo(handle, &mi);

	//manager->addMonitorNode(mi.dwFlags,
	//	mi.rcMonitor.top, mi.rcMonitor.left, mi.rcMonitor.bottom, mi.rcMonitor.right);
	manager->addMonitorNode(handle);

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

void MonitorManager::addMonitorNode(HMONITOR h)
{
	mMonitors.push_back(*new CMonitor(h));
}

void MonitorManager::clearMonitors()
{
	mMonitors.clear();
}

int MonitorManager::getWidth(int index)
{
	CMonitor* monitor = getMonitor(index);
	return monitor->getClientRight() - monitor->getClientLeft();
}

int MonitorManager::getHeight(int index)
{
	CMonitor* monitor = getMonitor(index);
	return monitor->getClientBottom() - monitor->getClientTop();
}

int MonitorManager::getClientWidth(int index)
{
	
	MONITORINFO mi;
	CMonitor* monitor = getMonitor(index);
	monitor->getInfo(&mi);

	return mi.rcWork.right - mi.rcWork.left;//monitor->getRight() - monitor->getLeft();
}

int MonitorManager::getClientHeight(int index)
{
	MONITORINFO mi;
	CMonitor* monitor = getMonitor(index);
	monitor->getInfo(&mi);
	return mi.rcWork.bottom - mi.rcWork.top;//monitor->getBottom() - monitor->getTop();
}

int MonitorManager::monitorFromPoint(int x, int y)
{

	list<CMonitor>::iterator itr;
	int i = 0;
	for (itr = mMonitors.begin(); itr != mMonitors.end(); itr++)
	{
		if (itr->getClientLeft() <= x && itr->getClientRight() > x &&
			itr->getClientTop() <= y && itr->getClientBottom() > y)
		{
			return i;
		}
		i++;
	}
	return -1;
}

int MonitorManager::monitorFromWindow(HWND hwnd)
{
	/*
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

	*/
	int index=0;
	HMONITOR hmonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
	if (!hmonitor)return -1;

	list<CMonitor>::iterator itr;
	for (itr = mMonitors.begin(); itr != mMonitors.end(); itr++)
	{
		if (itr->getHandle() == hmonitor)
		{
			return index;
		}
		index++;
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
		if (m->getClientTop() == itr->getClientBottom())
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
		if (m->getClientLeft() == itr->getClientRight())
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
		if (m->getClientRight() == itr->getClientLeft())
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
		if (m->getClientBottom() == itr->getClientTop())
		{
			return i;
		}
		i++;
	}

	return -1;
}
