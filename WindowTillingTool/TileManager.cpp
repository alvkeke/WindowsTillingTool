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
		// ��ȡÿ����Ļ����㡣
		biasy[i] = mMonManager->getMonitor(i)->getClientTop() + WINDOW_MARGIN_Y;
		biasx[i] = mMonManager->getMonitor(i)->getClientLeft() + WINDOW_MARGIN_X;
	}

	list<CWindow>::iterator itr;
	for (itr = mWinManager->getItrBegin(); !mWinManager->isItrEnd(itr); itr++)
	{
		// ��ȡÿ����ʾ���д��ڵĸ���
		int n = mMonManager->monitorFromWindow(itr->getHandle());
		if (n < 0 || n >= n_monitor) continue;	// �������������ݲ�����������
		if (!itr->isNormalShow()) continue;	//���ڲ�������������ʾ״̬
		if (itr->getHandle() == mHwndMain) continue; // ���������򴰿�
		// if (n>=0 && n<n_monitor) 
		n_window[n]++;
	}

	for (int i = 0; i < n_monitor; i++)
	{
		// ���ĳ����ʾ���µĴ�������Ϊ0������������ʾ����
		if (n_window[i] == 0) continue;
		winh[i] = mMonManager->getClientHeight(i);
		winw[i] = mMonManager->getClientWidth(i);
		add[i] = (winw[i] - WINDOW_MARGIN_X) / n_window[i];
		winh[i] -= WINDOW_MARGIN_Y * 2;
		winw[i] = add[i] - WINDOW_MARGIN_X;
	}

	// �����������ݣ��������ڴ�С��λ�á�
	for (itr = mWinManager->getItrBegin(); !mWinManager->isItrEnd(itr); itr++)
	{
		HWND hw = itr->getHandle();
		int n = mMonManager->monitorFromWindow(hw);
		if (n < 0 || n >= n_monitor) continue;	// �������������ݲ�����������
		if (!itr->isNormalShow()) continue;	//���ڲ�������������ʾ״̬
		if (n_window[n] == 0) continue;	// ��ʾ�����細�ڣ���������ʾ��
		if (hw == mHwndMain) continue; // ���������򴰿�

		ShowWindow(hw, SW_NORMAL);
		MoveWindow(hw, biasx[n], biasy[n], winw[n], winh[n], true);
		biasx[n] += add[n];
	}

	return;

}
