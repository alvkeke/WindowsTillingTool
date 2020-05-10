#include "WindowsManager.h"
#include <iostream>
using namespace std;

char filter_full[][IGNORE_WND_CLASS_MAX_LEN] = {
	// "ApplicationFrameWindow",	// 此类名为UWP应用的类名, 不能通过类名过滤
	"dummydwmlistenerwindow",
	"edgeuiinputtopwndclass",
	"windows.ui.core.corewindow",
	"progman",		// 
	"shell_traywnd",
	"internet explorer_hidden",
	"thumbnaildevicehelperwnd",
	"workerw",
	"multitaskingviewframe",
	"sysshadow",
	"shell_cortanaproxy",
	"tooltips_class32",
	"tasklistoverlaywnd",
	"tasklistthumbnailwnd",
	"listbox",
	""
};

char filter_part[][IGNORE_WND_CLASS_MAX_LEN] = {
	//"HwndWrapper",
	"#32768",
	"#32770"
};


CWindow::CWindow(HWND hwnd)
{
	this->mHwnd = hwnd;
}

HWND CWindow::getHandle()
{
	return this->mHwnd;
}

void CWindow::getPos(LPPOINT p)
{
	RECT r;
	GetWindowRect(mHwnd, &r);
	p->x = r.left;
	p->y = r.top;
}

void CWindow::setPos(int x, int y)
{
	SetWindowPos(mHwnd, NULL, x, y, 0, 0, SWP_NOSIZE);
}

void CWindow::getRect(LPRECT lpRect)
{
	GetWindowRect(mHwnd, lpRect);
}

void CWindow::setSize(int w, int h)
{
	SetWindowPos(mHwnd, NULL, 0, 0, w, h, SWP_NOMOVE);
}

void CWindow::setRect(LPRECT rect)
{
	MoveWindow(mHwnd, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, true);
}

void CWindow::getText(char* buf, int n_buf)
{
	GetWindowTextA(this->mHwnd, buf, n_buf);
}

void CWindow::getClassName(char* buf, int n_buf)
{
	RealGetWindowClassA(this->mHwnd, buf, n_buf);
}

boolean CWindow::isNormalShow()
{
	if (IsWindowVisible(mHwnd) && !IsIconic(mHwnd))
	{
		return true;
	}
	return false;
}



WindowsManager::WindowsManager()
{

	mAllWindows.clear();
	EnumDesktopWindows(NULL, EnumWndProc, (LPARAM)this);
}

void WindowsManager::refreshWindowList()
{

	if (!mAllWindows.empty()) mAllWindows.clear();
	EnumDesktopWindows(NULL, EnumWndProc, (LPARAM)this);
}

int WindowsManager::getAllWindowCount()
{

	return mAllWindows.size();
}


int WindowsManager::getAllShowWindowCount()
{
	int i = 0;
	list<CWindow>::iterator itr;
	
	for (itr = mAllWindows.begin(); itr != mAllWindows.end(); itr++)
	{
		if (itr->isNormalShow())
		{
			i++;
		}
	}

	return i;
}

CWindow* WindowsManager::getWindow(int index)
{
	list<CWindow>::iterator itr = mAllWindows.begin();

	if (mAllWindows.size() < index) return nullptr;
	
	for (; index > 0; index--)itr++;
	
	return &(*itr);
}

HWND WindowsManager::getHwnd(int index)
{
	return getWindow(index)->getHandle();
}

list<CWindow>* WindowsManager::getWindowList()
{
	return &mAllWindows;
}

list<CWindow>::iterator WindowsManager::getItrBegin()
{
	return mAllWindows.begin();
}

bool WindowsManager::isItrEnd(list<CWindow>::iterator itr)
{
	return itr==mAllWindows.end();
}

void WindowsManager::addWindowNode(HWND hwnd)
{
	mAllWindows.push_back(*new CWindow(hwnd));
}

void WindowsManager::clearWindows()
{
	mAllWindows.clear();
}

void WindowsManager::printWindowList()
{

	char buf[IGNORE_WND_CLASS_MAX_LEN];
	POINT p;
	list<CWindow>::iterator itr;
	for (itr = mAllWindows.begin(); itr != mAllWindows.end(); itr++)
	{
		itr->getPos(&p);
		itr->getText(buf, IGNORE_WND_CLASS_MAX_LEN);
		cout << p.x << ", " << p.y << "\t\t";
		cout << " : " << buf << " : ";
		itr->getClassName(buf, IGNORE_WND_CLASS_MAX_LEN);
		cout << buf << endl;
	}
}

BOOL WindowsManager::EnumWndProc(HWND hwnd, LPARAM lparam)
{
	char classbuf[IGNORE_WND_CLASS_MAX_LEN];
	bool v;
	RECT r;
	INT nCloaked;
	WindowsManager* wm = (WindowsManager*)lparam;

	RealGetWindowClassA(hwnd, classbuf, IGNORE_WND_CLASS_MAX_LEN);
	v = IsWindowVisible(hwnd);

	for (int i = 0; i < strlen(classbuf); i++)
	{
		classbuf[i] = tolower(classbuf[i]);
	}

	if (v)
	{
		if (strcmp(classbuf, "applicationframewindow") == 0)
		{
			// 判断UWP应用是否真正的显示
			DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &nCloaked, sizeof(INT));
			// v = !nCloaked;
			if (nCloaked) v = false;
		}
		else
		{
			for (int i = 0; i < sizeof(filter_full) / IGNORE_WND_CLASS_MAX_LEN; i++)
			{
				if (strcmp(classbuf, filter_full[i]) == 0)
				{
					v = false;
					break;
				}
			}
			for (int i = 0; i < sizeof(filter_part) / IGNORE_WND_CLASS_MAX_LEN; i++)
			{
				if (strstr(classbuf, filter_part[i]))
				{
					v = false;
					break;
				}
			}
		}
	}

	if (v)
	{
		wm->addWindowNode(hwnd);
	}

	return true;
}
