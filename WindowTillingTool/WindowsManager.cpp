#include "WindowsManager.h"
#include <iostream>
using namespace std;

char unneedwind[][IGNORE_WND_CLASS_MAX_LEN] = {
	// "ApplicationFrameWindow",	// 此类名为UWP应用的类名, 不能通过类名过滤
	"DummyDWMListenerWindow",
	"EdgeUiInputTopWndClass",
	"Windows.UI.Core.CoreWindow",
	"Progman",		// 
	"Shell_TrayWnd",
	"Internet Explorer_Hidden",
	"ThumbnailDeviceHelperWnd",
	"WorkerW",
	"MultitaskingViewFrame"
	""
};


WindowNode::WindowNode(HWND hwnd)
{
	this->mHwnd = hwnd;
	this->mNext = NULL;
	
}

WindowNode::~WindowNode()
{
	if (this->mNext)
	{
		delete this->mNext;
	}
}

HWND WindowNode::getHandle()
{
	return this->mHwnd;
}

void WindowNode::getPos(LPPOINT p)
{
	RECT r;
	GetWindowRect(mHwnd, &r);
	p->x = r.left;
	p->y = r.top;
}

void WindowNode::setPos(int x, int y)
{
	SetWindowPos(mHwnd, NULL, x, y, 0, 0, SWP_NOSIZE);
}

void WindowNode::getRect(LPRECT lpRect)
{
	GetWindowRect(mHwnd, lpRect);
}

void WindowNode::setSize(int w, int h)
{
	SetWindowPos(mHwnd, NULL, 0, 0, w, h, SWP_NOMOVE);
}

void WindowNode::setRect(LPRECT rect)
{
	MoveWindow(mHwnd, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, true);
}

void WindowNode::getText(char* buf, int n_buf)
{
	GetWindowTextA(this->mHwnd, buf, n_buf);
}

void WindowNode::getClassName(char* buf, int n_buf)
{
	RealGetWindowClassA(this->mHwnd, buf, n_buf);
}

WindowManager::WindowManager()
{
	WindowListHead* itr;
	this->mMonitors = new MonitorManager();
	mAllWindows = new WindowListHead(0);
	itr = mAllWindows;

	for (int i = 1; i < mMonitors->getMonitorCount(); i++)
	{
		itr->nextScreen = new WindowListHead(i);
		itr = itr->nextScreen;
	}
	EnumWindows(EnumWndProc, (LPARAM)this);
}

void WindowManager::refreshWindowList()
{
	WindowListHead* itr;
	if (mAllWindows) delete mAllWindows;
	mMonitors->refreshMonitors();

	mAllWindows = new WindowListHead(0);
	itr = mAllWindows;
	
	for (int i = 1; i < mMonitors->getMonitorCount(); i++)
	{
		itr->nextScreen = new WindowListHead(i);
		itr = itr->nextScreen;
	}
	EnumWindows(EnumWndProc, (LPARAM)this);
}

int WindowManager::getAllWindowCount()
{
	int i=0;
	for (WindowListHead* itr1 = mAllWindows; itr1 != NULL; itr1 = itr1->nextScreen)
	{
		for (WindowNode* itr = itr1->mWindowList; itr != NULL; itr = itr->mNext)
		{
			i++;
		}
	}
	return i;
}

WindowNode* WindowManager::getWindowNode(int screen, int index)
{
	WindowListHead* itr = mAllWindows;
	WindowNode* itr1;
	for (; screen > 0; screen--)
	{
		if (itr == NULL) return NULL;
		itr = itr->nextScreen;
	}
	itr1 = itr->mWindowList;
	for (; index > 0; index--)
	{
		if (itr1 == NULL) return NULL;
		itr1 = itr1->mNext;
	}

	return itr1;
}

HWND WindowManager::getWindow(int screen, int index)
{
	WindowListHead* itr = mAllWindows;
	WindowNode* itr1;
	for (; screen > 0; screen--)
	{
		if (itr == NULL) return NULL;
		itr = itr->nextScreen;
	}
	itr1 = itr->mWindowList;
	for (; index > 0; index--)
	{
		if (itr1 == NULL) return NULL;
		itr1 = itr1->mNext;
	}

	return itr1->getHandle();
}
WindowNode* WindowManager::getWindowList(int screen)
{
	WindowListHead* itr = mAllWindows;

	for (; screen > 0; screen--)
	{
		if (itr == NULL) return NULL;
		itr = itr->nextScreen;
	}

	return itr->mWindowList;
}
void WindowManager::addWindowNode(HWND hwnd)
{
	WindowListHead* itr;

	int index = mMonitors->monitorFromWindow(hwnd);
	itr = mAllWindows;
	for (; index > 0; index--)
	{
		if (itr == NULL) return;
		itr = itr->nextScreen;
	}

	if (itr->mWindowList == NULL)
	{
		itr->mWindowList = new WindowNode(hwnd);
		itr->mWindowTail = itr->mWindowList;
	}
	else
	{
		itr->mWindowTail->mNext = new WindowNode(hwnd);
		itr->mWindowTail = itr->mWindowTail->mNext;
	}
}

void WindowManager::clearWindows()
{
	delete mAllWindows;
	mAllWindows = NULL;
}

void WindowManager::printWindowList()
{
	POINT p;
	char buf[IGNORE_WND_CLASS_MAX_LEN];
	for (WindowListHead* itr1 = mAllWindows; itr1 != NULL; itr1 = itr1->nextScreen)
	{
		for (WindowNode* itr = itr1->mWindowList; itr != NULL; itr = itr->mNext)
		{
			itr->getPos(&p);
			itr->getText(buf, IGNORE_WND_CLASS_MAX_LEN);
			cout << p.x << ", " << p.y << "\t\t";
			cout << " : " << buf << " : ";
			itr->getClassName(buf, IGNORE_WND_CLASS_MAX_LEN);
			cout << buf << endl;
		}
	}
}

MonitorManager* WindowManager::getMonitormanager()
{
	return mMonitors;
}

BOOL WindowManager::EnumWndProc(HWND hwnd, LPARAM lparam)
{
	char classbuf[IGNORE_WND_CLASS_MAX_LEN];
	bool v;
	RECT r;
	INT nCloaked;
	WindowManager* wm = (WindowManager*)lparam;

	RealGetWindowClassA(hwnd, classbuf, IGNORE_WND_CLASS_MAX_LEN);
	v = IsWindowVisible(hwnd);

	if (v)
	{
		if (strcmp(classbuf, "ApplicationFrameWindow") == 0)
		{
			// 判断UWP应用是否真正的显示
			DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &nCloaked, sizeof(INT));
			// v = !nCloaked;
			if (nCloaked) v = false;
		}
		else
		{
			for (int i = 0; i < sizeof(unneedwind) / IGNORE_WND_CLASS_MAX_LEN; i++)
			{
				if (strcmp(classbuf, unneedwind[i]) == 0)
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

WindowListHead::WindowListHead(int iscreen)
{
	MonitorIndex = iscreen;
	nextScreen = NULL;
	mWindowList = NULL;
	mWindowTail = NULL;
}

WindowListHead::~WindowListHead()
{
	if (this->mWindowList)
	{
		delete mWindowList;
	}
	if (this->nextScreen)
	{
		if(nextScreen->nextScreen)
			delete nextScreen->nextScreen;
	}
}
