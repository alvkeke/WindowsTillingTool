#include "WindowsManager.h"
#include <iostream>
using namespace std;

char filter_full[][IGNORE_WND_CLASS_MAX_LEN] = {
	// "ApplicationFrameWindow",	// 此类名为UWP应用的类名, 不能通过类名过滤
	// "dummydwmlistenerwindow",
	// "edgeuiinputtopwndclass",
	// "windows.ui.core.corewindow",
	// "progman",		// 
	// "shell_traywnd",
	// "internet explorer_hidden",
	// "thumbnaildevicehelperwnd",
	// "workerw",
	// "multitaskingviewframe",
	// "sysshadow",
	// "shell_cortanaproxy",
	// "tooltips_class32",
	// "tasklistoverlaywnd",
	// "tasklistthumbnailwnd",
	// "syslink",
	// "sysipaddress32",
	// "listbox",
	// "syslistview32",
	// "systreeview32",
	// "msctls_progress32",
	// "static",
	// "button",
	// "comboxex32",
	// "combox",
	// "toolbarwindow32",
	""
};

char filter_part[][IGNORE_WND_CLASS_MAX_LEN] = {
	//"HwndWrapper",
	"#32768",	//	弹出菜单
	//"#32770",
	"#32774",	//	提示弹出文字框
	
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
	MoveWindow(mHwnd, rect->left, rect->top, 
		rect->right - rect->left, rect->bottom - rect->top, true);
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
	EnumDesktopWindows(NULL, EnumWndProc, (LPARAM)this);
	clearOutdateWindows();
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

	advance(itr, index);
	
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

void WindowsManager::clearOutdateWindows()
{
	list<CWindow>::iterator itr;

	for (itr = mAllWindows.begin(); itr != mAllWindows.end(); itr++)
	{
		if (!IsWindow(itr->getHandle()))
		{
			itr = mAllWindows.erase(itr);
			itr--;
		}
		else
		{
			if (!IsWindowVisible(itr->getHandle()))
			{
				itr = mAllWindows.erase(itr);
				itr--;
			}
		}
	}
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
	RECT r;
	list<CWindow>::iterator itr;
	for (itr = mAllWindows.begin(); itr != mAllWindows.end(); itr++)
	{
		itr->getRect(&r);
		itr->getText(buf, IGNORE_WND_CLASS_MAX_LEN);
		cout << r.left << "," << r.top << ": " << r.right << "," << r.bottom << "\t\t";
		cout << " : " << buf << " : ";
		itr->getClassName(buf, IGNORE_WND_CLASS_MAX_LEN);
		cout << buf << endl;
	}
}

BOOL WindowsManager::EnumWndProc(HWND hwnd, LPARAM lparam)
{
	char classbuf[IGNORE_WND_CLASS_MAX_LEN];
	RECT r;
	INT nCloaked;
	WindowsManager* wm;
	
	wm = (WindowsManager*)lparam;

	// 判断窗口是否已经存在与列表中，如果存在则跳过
	list<CWindow>::iterator itr;
	for (itr = wm->mAllWindows.begin(); itr != wm->mAllWindows.end(); itr++)
	{
		if (itr->getHandle() == hwnd)
		{
			return true;
		}
	}

	RealGetWindowClassA(hwnd, classbuf, IGNORE_WND_CLASS_MAX_LEN);

	if (!IsWindowVisible(hwnd)) return true;

	for (int i = 0; i < strlen(classbuf); i++)
	{
		classbuf[i] = tolower(classbuf[i]);
	}

	if (strcmp(classbuf, "applicationframewindow") == 0)
	{
		// 判断UWP应用是否真正的显示
		DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &nCloaked, sizeof(INT));
		if (nCloaked) return true;	// nCloaked为真则说明窗口没有真正显示
	}
	else
	{
		for (int i = 0; i < sizeof(filter_full) / IGNORE_WND_CLASS_MAX_LEN; i++)
		{
			if (strcmp(classbuf, filter_full[i]) == 0)
			{
				return true;
			}
		}
		for (int i = 0; i < sizeof(filter_part) / IGNORE_WND_CLASS_MAX_LEN; i++)
		{
			if (strstr(classbuf, filter_part[i]))
			{
				return true;
			}
		}
	}

	int style = GetWindowLong(hwnd, GWL_STYLE);
	if (! (style & WS_OVERLAPPEDWINDOW)) return true;
	
	wm->addWindowNode(hwnd);
	
	return true;
}
