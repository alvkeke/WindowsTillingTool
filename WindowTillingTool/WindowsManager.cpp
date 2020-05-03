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

HWND WindowNode::getHwnd()
{
	return mHwnd;
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
	mWindowList = NULL;
	mWindowTail = NULL;
	EnumWindows(EnumWndProc, (LPARAM)this);
}

void WindowManager::refreshWindowList()
{
	if (mWindowList) delete mWindowList;
	mWindowList = NULL;
	mWindowTail = NULL;
	EnumWindows(EnumWndProc, (LPARAM)this);

}

int WindowManager::getWindowCount()
{
	int i=0;
	for (WindowNode* itr = mWindowList; itr != NULL; itr = itr->mNext)
	{
		i++;
	}
	return i;
}

HWND WindowManager::getWindow(int index)
{
	WindowNode* itr = mWindowList;
	for (; index > 0; index--)
	{
		if (itr == NULL) return NULL;
		itr = itr->mNext;
	}

	return itr->getHwnd();
}
void WindowManager::addWindowNode(HWND hwnd)
{
	if (mWindowList == NULL)
	{
		mWindowList = new WindowNode(hwnd);
		mWindowTail = mWindowList;
	}
	else
	{
		mWindowTail->mNext = new WindowNode(hwnd);
		mWindowTail = mWindowTail->mNext;
	}
}

void WindowManager::clearWindows()
{
	delete mWindowList;
	mWindowList = NULL;
	mWindowTail = NULL;
}

void WindowManager::printWindowList()
{
	POINT p;
	char buf[IGNORE_WND_CLASS_MAX_LEN];
	for (WindowNode* itr = mWindowList; itr != NULL; itr = itr->mNext)
	{
		itr->getPos(&p);
		itr->getText(buf, IGNORE_WND_CLASS_MAX_LEN);
		cout << p.x << ", " << p.y << "\t\t" ;
		cout << " : " << buf << " : ";
		itr->getClassName(buf, IGNORE_WND_CLASS_MAX_LEN);
		cout << buf << endl;
	}
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
