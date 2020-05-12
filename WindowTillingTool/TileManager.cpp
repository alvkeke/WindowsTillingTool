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

bool TileManager::refreshWinList()
{
	return mWinManager->refreshWindowList();
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

void TileManager::addClassBlock(string classname)
{
	mClassBlockList.push_back(classname);
}

void TileManager::addTextBlock(string text)
{
	mTextBlockList.push_back(text);
}

void TileManager::addBothBlock(string classname, string text)
{
	mBothBlockList_Text.push_back(text);
	mBothBlockList_Class.push_back(classname);
}

void TileManager::addHwndBlock(HWND hwnd)
{
	mHwndBlockList.push_back(hwnd);
}

void TileManager::clearAllBlock()
{
	mClassBlockList.clear();
	mTextBlockList.clear();
	mBothBlockList_Text.clear();
	mHwndBlockList.clear();
}

bool TileManager::checkBlock(CWINITR itr)
{
	char buffer[BUFFER_SIZE];

	list<string>::iterator itrblock;
	list<HWND>::iterator itrh;
	string stext, sclass;
	//HWND hwndblock;

	for (itrh = mHwndBlockList.begin(); itrh != mHwndBlockList.end(); itrh++)
	{
		if (*itrh == itr->getHandle())
		{
			return true;
		}
	}

	itr->getClassName(buffer, BUFFER_SIZE);
	sclass = buffer;
	for (itrblock = mClassBlockList.begin(); itrblock != mClassBlockList.end(); itrblock++)
	{
		if (sclass == *itrblock)
		{
			return true;
		}
	}

	itr->getText(buffer, BUFFER_SIZE);
	sclass = buffer;
	for (itrblock = mTextBlockList.begin(); itrblock != mTextBlockList.end(); itrblock++)
	{
		if (sclass == *itrblock)
		{
			return true;
		}
	}

	return false;
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


	// 显示器
	for (int i = 0; i < n_monitor; i++)
	{
		n_window[i] = 0;
		// 获取每个屏幕的起点。
		biasy[i] = mMonManager->getMonitor(i)->getClientTop() + WINDOW_MARGIN_Y;
		biasx[i] = mMonManager->getMonitor(i)->getClientLeft() + WINDOW_MARGIN_X;
	}

	// 窗口第一次
	list<CWindow>::iterator itr;
	for (itr = mWinManager->getItrBegin(); !mWinManager->isItrEnd(itr); itr++)
	{
		// 获取每个显示器中窗口的个数
		int n = mMonManager->monitorFromWindow(itr->getHandle());
		if (n < 0 || n >= n_monitor) continue;	// 数据与已有数据不符，则跳过
		if (!itr->isNormalShow()) continue;	//窗口并不处于正常显示状态
		// if (itr->getHandle() == mHwndMain) continue; // 跳过本程序窗口

		if (checkBlock(itr)) continue;

		n_window[n]++;
	}

	// 显示器第二次
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

	// 窗口第三次
	// 根据已有数据，调整窗口大小与位置。
	for (itr = mWinManager->getItrBegin(); !mWinManager->isItrEnd(itr); itr++)
	{
		HWND hw = itr->getHandle();
		int n = mMonManager->monitorFromWindow(hw);
		if (n < 0 || n >= n_monitor) continue;	// 数据与已有数据不符，则跳过
		if (!itr->isNormalShow()) continue;	//窗口并不处于正常显示状态
		if (n_window[n] == 0) continue;	// 显示器下午窗口，跳过该显示器
		// if (hw == mHwndMain) continue; // 跳过本程序窗口
		if (checkBlock(itr))continue;

		ShowWindow(hw, SW_NORMAL);
		MoveWindow(hw, biasx[n], biasy[n], winw[n], winh[n], true);
		biasx[n] += add[n];
	}

	delete[] n_window;
	delete[] winw;
	delete[] winh;
	delete[] biasy;
	delete[] biasx;
	delete[] add;

	return;
}
