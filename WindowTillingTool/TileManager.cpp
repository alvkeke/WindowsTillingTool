#include "TIleManager.h"

TileManager::TileManager(HWND hMainWnd)
{
	mHwndMain = hMainWnd;
	mWinManager = new WindowsManager();
	mMonManager = new MonitorManager();
	mPaddingX = WINDOW_PADDING_X;
	mPaddingY = WINDOW_PADDING_Y;
	refreshScnList();
}

int TileManager::getScnCount()
{
	return mMonManager->getMonitorCount();
}

void TileManager::refreshScnList()
{
	mMonManager->refreshMonitors();
	mScnInfo.clear();
	CMONITR itr;
	itr = mMonManager->getItrBegin();
	for (; !mMonManager->isItrEnd(itr); itr++)
	{
		mScnInfo.push_back(*new TileScnInfo(itr->getHandle()));
	}
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

bool TileManager::isWinInList(HWND hwnd)
{
	return mWinManager->isWinInList(hwnd);
}

void TileManager::printWinList()
{
	mWinManager->printWindowList();
}

void TileManager::toggleWinTmpFull(HWND hwnd)
{
	int iMon = mMonManager->monitorFromWindow(hwnd);
	TileScnInfo* scn = getScnInfo(iMon);
	if (scn->isAnyWinSetFull())
	{
		if (!scn->wasWinSetFull(hwnd))
		{
			scn->cancelFullWin();
			scn->setFullWin(hwnd);
		}
		else
		{
			scn->cancelFullWin();
		}
	}
	else
	{
		scn->setFullWin(hwnd);
	}
	tileWindows();
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

void TileManager::addClassPartBlock(string classname)
{
	mClassPartBlockList.push_back(classname);
}

void TileManager::addTextPartBlock(string text)
{
	mTextPartBlockList.push_back(text);
}

void TileManager::clearAllBlock()
{
	mClassBlockList.clear();
	mTextBlockList.clear();
	mClassPartBlockList.clear();
	mTextPartBlockList.clear();
	mBothBlockList_Text.clear();
	mBothBlockList_Class.clear();
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
	if (!mClassPartBlockList.empty())
	{
		for (itrblock = mClassPartBlockList.begin(); itrblock != mClassPartBlockList.end(); itrblock++)
		{
			if (!itrblock->empty() && sclass.find(*itrblock) != string::npos)
			{
				return true;
			}
		}
	}
	for (itrblock = mClassBlockList.begin(); itrblock != mClassBlockList.end(); itrblock++)
	{
		if (sclass == *itrblock)
		{
			return true;
		}
	}

	itr->getText(buffer, BUFFER_SIZE);
	stext = buffer;
	if (!mTextPartBlockList.empty())
	{
		for (itrblock = mTextPartBlockList.begin(); itrblock != mTextPartBlockList.end(); itrblock++)
		{
			if (!itrblock->empty() && stext.find(*itrblock) != string::npos)
			{
				return true;
			}
		}
	}
	for (itrblock = mTextBlockList.begin(); itrblock != mTextBlockList.end(); itrblock++)
	{
		if (stext == *itrblock)
		{
			return true;
		}
	}

	if (mBothBlockList_Class.size() != mBothBlockList_Text.size())
	{
		MessageBox(mHwndMain, "部分屏蔽数据出错，如需重新启用，请重新加载配置文件。", "错误", MB_ICONWARNING);
		mBothBlockList_Class.clear();
		mBothBlockList_Text.clear();
		return false;
	}

	list<string>::iterator itrtext = mBothBlockList_Text.begin();
	itrblock = mBothBlockList_Class.begin();
	for (; itrblock != mBothBlockList_Class.end(); itrblock++)
	{
		if (itrtext == mBothBlockList_Text.end()) break;

		if (sclass == *itrblock && stext == *itrtext)
		{
			return true;
		}
		itrtext++;
	}
	

	return false;
}

void TileManager::moveFocusWindowLeft()
{
	HWND h = GetForegroundWindow();
	int iMon = mMonManager->monitorFromWindow(h);
	if (iMon < 0) return;
	TILESCNITR itr = mScnInfo.begin();
	advance(itr, iMon);
	itr->stepWinLeft(h);

}

void TileManager::moveFocusWindowRight()
{
	HWND h = GetForegroundWindow();
	int iMon = mMonManager->monitorFromWindow(h);
	if (iMon < 0) return;
	TILESCNITR itr = mScnInfo.begin();
	advance(itr, iMon);
	itr->stepWinRight(h);
}

void TileManager::setTilePadding(int x, int y)
{
	if (x >= 0) mPaddingX = x;
	if (y >= 0) mPaddingY = y;
}

TileScnInfo* TileManager::getScnInfo(int iScn)
{
	TILESCNITR itr;
	if (iScn >= mScnInfo.size()) return nullptr;
	itr = mScnInfo.begin();
	advance(itr, iScn);
	return &(*itr);
}

TileScnInfo* TileManager::getScnInfo(HMONITOR handle)
{
	TILESCNITR itr;
	itr = mScnInfo.begin();
	for (; itr != mScnInfo.end(); itr++)
	{
		if (itr->getHandle() == handle)return &(*itr);
	}
	return nullptr;
}

void TileManager::addWinInfo(int iScn, HWND hwnd)
{
	TILESCNITR itr = mScnInfo.begin();
	if (iScn >= mScnInfo.size()) return;
	advance(itr, iScn);
	itr->addWin(hwnd);
}

void TileManager::assignWinInfoList()
{

	// 清除失效数据
	int iScn = 0;
	for (TILESCNITR scnInfoIt = mScnInfo.begin(); scnInfoIt != mScnInfo.end(); scnInfoIt++)
	{
		for (TILEWINITR winInfoIt = scnInfoIt->getWinInfoItr(); 
			!scnInfoIt->isItrEnd(winInfoIt); winInfoIt++)
		{
			if (!IsWindow(winInfoIt->getHandle())
				|| iScn != mMonManager->monitorFromWindow(winInfoIt->getHandle())
				|| IsIconic(winInfoIt->getHandle())
				|| !IsWindowVisible(winInfoIt->getHandle()))
			{
				 winInfoIt = scnInfoIt->delWin(winInfoIt);

				 if(winInfoIt != scnInfoIt->getWinInfoItr()) winInfoIt--;
				 if (scnInfoIt->isItrEnd(winInfoIt)) break;
			}
		}

		iScn++;
	}

	// 添加窗口到列表中
	for (CWINITR winIt = mWinManager->getItrBegin(); !mWinManager->isItrEnd(winIt); winIt++)
	{
		if (!winIt->isNormalShow()) continue;
		if (checkBlock(winIt)) continue;

		int iScn = mMonManager->monitorFromWindow(winIt->getHandle());
		TileScnInfo* scninfo = getScnInfo(iScn);
		if (!scninfo) continue;
		int iWin = scninfo->indexWin(winIt->getHandle());
		if (iWin >= 0)continue;		//已存在
		scninfo->addWin(winIt->getHandle());
	}

}

//void TileManager::tileWindows2()
//{
//	int n_monitor = mMonManager->getMonitorCount();
//
//	int* n_window = new int[n_monitor];
//	int* winw = new int[n_monitor];
//	int* winh = new int[n_monitor];
//	int* biasy = new int[n_monitor];
//	int* biasx = new int[n_monitor];
//	int* add = new int[n_monitor];
//
//	// 显示器
//	for (int i = 0; i < n_monitor; i++)
//	{
//		n_window[i] = 0;
//		// 获取每个屏幕的起点。
//		biasy[i] = mMonManager->getMonitor(i)->getClientTop() + mPaddingY;
//		biasx[i] = mMonManager->getMonitor(i)->getClientLeft() + mPaddingX;
//	}
//
//	// 窗口第一次
//	list<CWindow>::iterator itr;
//	for (itr = mWinManager->getItrBegin(); !mWinManager->isItrEnd(itr); itr++)
//	{
//		// 获取每个显示器中窗口的个数
//		int n = mMonManager->monitorFromWindow(itr->getHandle());
//		if (n < 0 || n >= n_monitor) continue;	// 数据与已有数据不符，则跳过
//		if (!itr->isNormalShow()) continue;	//窗口并不处于正常显示状态
//		if (checkBlock(itr)) continue;
//
//		n_window[n]++;
//	}
//
//	// 显示器第二次
//	for (int i = 0; i < n_monitor; i++)
//	{
//		// 如果某个显示器下的窗口数量为0，则跳过该显示器。
//		if (n_window[i] == 0) continue;
//		winh[i] = mMonManager->getClientHeight(i);
//		winw[i] = mMonManager->getClientWidth(i);
//		add[i] = (winw[i] - mPaddingX) / n_window[i];
//		winh[i] -= mPaddingY * 2;
//		winw[i] = add[i] - mPaddingX;
//	}
//
//	// 窗口第二次，根据已有数据，调整窗口大小与位置。
//	for (itr = mWinManager->getItrBegin(); !mWinManager->isItrEnd(itr); itr++)
//	{
//		HWND hw = itr->getHandle();
//		int n = mMonManager->monitorFromWindow(hw);
//		if (n < 0 || n >= n_monitor) continue;	// 数据与已有数据不符，则跳过
//		if (!itr->isNormalShow()) continue;	//窗口并不处于正常显示状态
//		if (n_window[n] == 0) continue;	// 显示器下无窗口，跳过该显示器
//		if (checkBlock(itr)) continue;
//
//		if (n_window[n] == 1)
//		{
//			ShowWindow(hw, SW_MAXIMIZE);
//			continue;
//		}
//		ShowWindow(hw, SW_NORMAL);
//		MoveWindow(hw, biasx[n], biasy[n], winw[n], winh[n], true);
//		biasx[n] += add[n];
//	}
//
//	delete[] n_window;
//	delete[] winw;
//	delete[] winh;
//	delete[] biasy;
//	delete[] biasx;
//	delete[] add;
//
//	return;
//}

void TileManager::tileWindows()
{
	int iScn = 0;
	int biasx, biasy, addx, addy, winwid, winhei;
	int nWindows;
	CMONITR mon;

	assignWinInfoList();

	mon = mMonManager->getItrBegin();

	for (TILESCNITR itr = mScnInfo.begin(); itr != mScnInfo.end(); itr++)
	{
		nWindows = itr->getWinCount();

		if (nWindows == 0)
		{
			mon++;
			continue;
		}
		else if (nWindows == 1)
		{
			TILEWINITR win = itr->getWinInfoItr();
			ShowWindow(win->getHandle(), SW_MAXIMIZE);
			win->updateData();
		}
		else
		{
			biasy = mon->getClientTop() + mPaddingY;
			biasx = mon->getClientLeft() + mPaddingX;

			addx = (mon->getClientRight() - mon->getClientLeft() - mPaddingX) / nWindows;
			addy = mon->getClientBottom() - mon->getClientTop() - mPaddingY;	//暂时用不上

			winwid = addx - mPaddingX;
			winhei = addy - mPaddingY;	// 暂时用不上

			for (TILEWINITR win = itr->getWinInfoItr(); !itr->isItrEnd(win); win++)
			{
				
				if (win->isDataDifferent(itr->wasWinSetFull(win->getHandle()), 
					biasx, biasy, winwid, winhei)
					|| win->isDataChanged())
				{
					win->move(biasx, biasy, winwid, winhei);
					win->updateData();
				}

				biasx += addx;
			}
		}

		mon++;
		if (mMonManager->isItrEnd(mon))return;
	}
}

TileWinInfo::TileWinInfo(HWND hwnd)
{
	mhWnd = hwnd;
	mIsUserSetMaxed = false;
	mSetWid = -1;
	mSetHei = -1;
	mOldX = mOldY = mOldW = mOldH = -1;
	mOldZoomedState = false;
	//mPosChanged = false;
}

HWND TileWinInfo::getHandle()
{
	return mhWnd;
}

void TileWinInfo::updateData()
{
	RECT r;
	GetWindowRect(mhWnd, &r);
	mOldZoomedState = IsZoomed(mhWnd);
	if (mOldZoomedState) return;

	mOldX = r.left;
	mOldY = r.top;
	mOldW = r.right - r.left;
	mOldH = r.bottom - r.top;
}

void TileWinInfo::setWidth(int w)
{
	mSetWid = w;
}

void TileWinInfo::setHeight(int h)
{
	mSetHei = h;
}

bool TileWinInfo::isDataChanged()
{
	RECT r;

	//if (mPosChanged) return true;
	GetWindowRect(mhWnd, &r);
	if (r.left != mOldX) return true;
	if (r.top != mOldY) return true;
	if (r.right - r.left != mOldW) return true;
	if (r.bottom - r.top != mOldH) return true;

	return false;
}

bool TileWinInfo::isDataDifferent(bool zoomState, int x, int y, int w, int h)
{
	if (mOldZoomedState != zoomState) return true;
	if (x != mOldX)return true;
	if (y != mOldY)return true;
	if (w != mOldW)return true;
	if (h != mOldH)return true;

	return false;
}

bool TileWinInfo::isUserSetMaxed()
{
	return mIsUserSetMaxed;
}

void TileWinInfo::usersetMaxed(bool foo)
{
	mIsUserSetMaxed = foo;
}

//void TileWinInfo::indexChanged()
//{
//	mPosChanged = true;
//}

void TileWinInfo::move(int x, int y, int w, int h)
{
	if (mIsUserSetMaxed)
	{
		SetWindowPos(mhWnd, HWND_TOPMOST, x, y, w, h, SWP_NOMOVE|SWP_NOSIZE);
		ShowWindow(mhWnd, SW_MAXIMIZE);
	}
	else
	{
		ShowWindow(mhWnd, SW_SHOWNORMAL);
		SetWindowPos(mhWnd, HWND_BOTTOM, x, y, w, h, NULL);
	}
}

TileScnInfo::TileScnInfo(HMONITOR hscn)
{
	mhScn = hscn;
	mIndexFullWin = -1;
	mWins.clear();
}

HMONITOR TileScnInfo::getHandle()
{
	return mhScn;
}

void TileScnInfo::addWin(HWND hwnd)
{
	if (IsWindow(hwnd))
	{
		mWins.push_back(*new TileWinInfo(hwnd));
	}
}

void TileScnInfo::delWin(HWND hwnd)
{
	TILEWINITR itr;
	for (itr = mWins.begin(); itr != mWins.end(); itr++)
	{
		if (itr->getHandle() == hwnd)
		{
			mWins.erase(itr);
			return;
		}
	}
}

TILEWINITR TileScnInfo::delWin(TILEWINITR itr)
{
	TILEWINITR itr2 = mWins.erase(itr);
	return itr2;
}

int TileScnInfo::indexWin(HWND hwnd)
{
	int i = 0;
	TILEWINITR itr = mWins.begin();
	for (; itr != mWins.end(); itr++)
	{
		if (itr->getHandle() == hwnd)
		{
			return i;
		}
		i++;
	}

	return -1;
}

int TileScnInfo::getWinCount()
{
	return mWins.size();
}

void TileScnInfo::clearWins()
{
	mWins.clear();
}

void TileScnInfo::stepWinLeft(HWND hwnd)
{
	TILEWINITR itrsave;
	TILEWINITR itr; 
	
	itrsave = mWins.begin();
	if (itrsave->getHandle() == hwnd) return;
	itr = mWins.begin();
	itr++;

	for (; itr != mWins.end(); itr++)
	{
		if (itr->getHandle() == hwnd)
		{
			swap(*itrsave, *itr);
			//itr->indexChanged();
			//itrsave->indexChanged();
			return;
		}
		itrsave = itr;
	}
}

void TileScnInfo::stepWinRight(HWND hwnd)
{
	TILEWINITR itrsave;
	TILEWINITR itr;

	itr = mWins.begin();
	itrsave = itr;		// 0
	itrsave++;			// 1

	for (; itrsave != mWins.end(); itr++)
	{
		if (itr->getHandle() == hwnd)
		{
			swap(*itrsave, *itr);
			//itr->indexChanged();
			//itrsave->indexChanged();
			return;
		}
		itrsave++;
	}
}

bool TileScnInfo::wasWinSetFull(HWND hwnd)
{
	return mIndexFullWin == indexWin(hwnd);
}

bool TileScnInfo::wasWinSetFull(int index)
{
	return index == mIndexFullWin;
}

void TileScnInfo::setFullWin(HWND hwnd)
{
	TILEWINITR itr;
	if (mIndexFullWin >= 0)return;	// 保护
	for (itr = mWins.begin(); itr != mWins.end(); itr++)
	{
		if (itr->getHandle() == hwnd)
		{
			itr->usersetMaxed(true);
			mIndexFullWin = indexWin(hwnd);
			return;
		}
	}
}

void TileScnInfo::cancelFullWin()
{
	TILEWINITR itr = mWins.begin();
	advance(itr, mIndexFullWin);
	itr->usersetMaxed(false);
	mIndexFullWin = -1;
}

bool TileScnInfo::isAnyWinSetFull()
{
	return mIndexFullWin >= 0;
}

TILEWINITR TileScnInfo::getWinInfoItr()
{
	return mWins.begin();
}

bool TileScnInfo::isItrEnd(TILEWINITR itr)
{
	return itr == mWins.end();
}
