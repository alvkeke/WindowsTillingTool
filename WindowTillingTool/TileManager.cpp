#include "TIleManager.h"

TileManager::TileManager(HWND hMainWnd)
{
	mHwndMain = hMainWnd;
	mWinManager = new WindowsManager();
	mMonManager = new MonitorManager();

}

int TileManager::getScnCount()
{
	return mMonManager->getMonitorCount();
}

void TileManager::refreshScnList()
{
	mMonManager->refreshMonitors();
}

CMonitor* TileManager::getScreen(int index)
{
	return mMonManager->getMonitor(index);
}

int TileManager::screenFromWindow(HWND hwnd)
{
	return mMonManager->monitorFromWindow(hwnd);
}

list<CMonitor>::iterator TileManager::getScnListItr()
{
	return mMonManager->getItrBegin();
}

bool TileManager::isItrEnd(list<CMonitor>::iterator itr)
{
	return mMonManager->isItrEnd(itr);
}

void TileManager::refreshWinList()
{
	mWinManager->refreshWindowList();
}

list<CWindow> TileManager::getWinListInScn(int imonitor)
{
	list<CWindow> l;
	list<CWindow>::iterator itr;
	for (itr = mWinManager->getItrBegin(); !mWinManager->isItrEnd(itr); itr++)
	{
		if (mMonManager->monitorFromWindow(itr->getHandle()) == imonitor)
		{
			l.push_back(*itr);
		}
	}
	return l;
}

list<CWindow>::iterator TileManager::getWinListItr()
{
	return mWinManager->getItrBegin();
}

bool TileManager::isItrEnd(list<CWindow>::iterator itr)
{
	return mWinManager->isItrEnd(itr);
}

void TileManager::printWinList()
{
	mWinManager->printWindowList();
}

void TileManager::tileWindows()
{
	int n_monitor = mMonManager->getMonitorCount();

	int* n_window = new int[n_monitor];
	int* winw = new int[n_monitor];
	int* winh = new int[n_monitor];
	int* biasy = new int[n_monitor];
	int* biasx = new int[n_monitor];
	int* add = new int[n_monitor];

	for (int i = 0; i < n_monitor; i++)
	{
		n_window[i] = 0;
		// 获取每个屏幕的起点。
		biasy[i] = mMonManager->getMonitor(i)->getClientTop() + WINDOW_MARGIN_Y;
		biasx[i] = mMonManager->getMonitor(i)->getClientLeft() + WINDOW_MARGIN_X;
	}

	list<CWindow>::iterator itr;
	for (itr = mWinManager->getItrBegin(); !mWinManager->isItrEnd(itr); itr++)
	{
		// 获取每个显示器中窗口的个数
		int n = mMonManager->monitorFromWindow(itr->getHandle());
		if (n < 0 || n >= n_monitor) continue;	// 数据与已有数据不符，则跳过
		if (!itr->isNormalShow()) continue;	//窗口并不处于正常显示状态
		if (itr->getHandle() == mHwndMain) continue; // 跳过本程序窗口
		// if (n>=0 && n<n_monitor) 
		n_window[n]++;
	}

	for (int i = 0; i < n_monitor; i++)
	{
		// 如果某个显示器下的窗口数量为0，则跳过该显示器。
		if (n_window[i] == 0) continue;
		winh[i] = mMonManager->getClientHeight(i);
		winw[i] = mMonManager->getClientWidth(i);
		add[i] = (winw[i] - WINDOW_MARGIN_X) / n_window[i];
		winh[i] -= WINDOW_MARGIN_Y * 2;
		winw[i] = add[i] - WINDOW_MARGIN_X;
	}

	// 根据已有数据，调整窗口大小与位置。
	for (itr = mWinManager->getItrBegin(); !mWinManager->isItrEnd(itr); itr++)
	{
		HWND hw = itr->getHandle();
		int n = mMonManager->monitorFromWindow(hw);
		if (n < 0 || n >= n_monitor) continue;	// 数据与已有数据不符，则跳过
		if (!itr->isNormalShow()) continue;	//窗口并不处于正常显示状态
		if (n_window[n] == 0) continue;	// 显示器下午窗口，跳过该显示器
		if (hw == mHwndMain) continue; // 跳过本程序窗口

		ShowWindow(hw, SW_NORMAL);
		MoveWindow(hw, biasx[n], biasy[n], winw[n], winh[n], true);
		biasx[n] += add[n];
	}

	return;

}
