#include "main.h"
#include "TileManager.h"
#include "TrayIcon.h"
#include "resource.h"
#include "BlockSettingWindow.h"

HINSTANCE mhInstance;

HWND hConsoleWnd;
HWND hWnd;
HMENU hMenu;

HANDLE hTimer;

HWND hCBMonitors;
HWND hLBWindows;
HWND hETText;
HWND hETClass;
HWND hETHwnd;
HWND hETPos;
HWND hETSize;

list<CWindow> winlist;
TileManager* tileManager;

bool bPrintThreadLock = false;
bool bConsolePrint;
bool isThreadRunning;


DWORD WINAPI timerProc(LPVOID interval)
{
	long interval_ms = (DWORD)interval;

	while (true)
	{
		
		if (!isThreadRunning) return NULL;
		if (tileManager)
		{
			if (tileManager->refreshWinList())
			{
				updateWinListBox();
			}
			tileManager->tileWindows();
			
			if (bConsolePrint)
			{
				bPrintThreadLock = true;	// 线程锁
				tileManager->printWinList();
				bPrintThreadLock = false;
			}
			
		}
		Sleep(interval_ms);
		if (bConsolePrint) system("cls");
	}

	return NULL;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	switch (msg)
	{
	case WM_CREATE:
	{
		char buf[BUFFER_SIZE];

		bPrintThreadLock = false;

		hMenu = LoadMenu(mhInstance, MAKEINTRESOURCE(IDR_MENU1));

		initCompoents(hWnd);
		initTrayIcon(hWnd);
		ReadjustWindow(hWnd, 560, 470);

		tileManager = new TileManager(hWnd);
		tileManager->addClassBlock(APP_WIN_CLASS_MAIN);
		tileManager->addClassBlock(APP_WIN_CLASS_BLOCK);
		tileManager->addTextBlock(APP_TITLE_CONSOLE);

		for (int i = 0; i < tileManager->getScnCount(); i++)
		{
			CMonitor* m = tileManager->getScreen(i);
			snprintf(buf, BUFFER_SIZE,  "%d [%d x %d]\0", i,
				m->getClientRight()-m->getClientLeft(), m->getClientBottom()-m->getClientTop());

			cbAddItem(hCBMonitors, buf);
			if (i == 0)
			{
				SendMessage(hCBMonitors, CB_SELECTSTRING, 0, (WPARAM)buf);
				updateWinListBox();
			}
		}

		isThreadRunning = true;
		hTimer = CreateThread(NULL, 0, timerProc, (LPVOID)TIMER_INTERVAL_MS, NULL, NULL);
	}
		break;

	case WM_RBUTTONUP:
	{
		popupMenu(hWnd);
	}
		break;

	case WM_COMMAND:
	{
		UINT id = LOWORD(wparam);

		if (id == ID_CB_MONITOR)
		{
			// if (HIWORD(wparam) == CBN_SELCHANGE)
			{
				updateWinListBox();
			}
		}
		else if (id == ID_LB_WINDOWS)
		{
			UINT action = HIWORD(wparam);
			if (action == LBN_SELCHANGE)
			{
				updateWindowInfo();
			}
			else if (action == LBN_DBLCLK)
			{
				char buf1[BUFFER_SIZE];
				char buf2[BUFFER_SIZE];

				int winindex = lbGetSelectItem();
				if (winindex < 0) break;

				list<CWindow>::iterator win = winlist.begin();
				advance(win, winindex);

				win->getClassName(buf1, BUFFER_SIZE);
				win->getText(buf2, BUFFER_SIZE);
				HWND htmp = createBlkSetWnd(mhInstance, hWnd);
				SendMessage(htmp, WM_BLOCK_SETSTRINFO, (WPARAM)buf1, (LPARAM)buf2);
				SendMessage(htmp, WM_BLOCK_SETHWND, (WPARAM)win->getHandle(), NULL);
			}
		}
		else if (id == IDSM_EXIT)
		{
			delTrayIcon();
			PostQuitMessage(0);
		}
		else if (id == IDSM_ENABLE_TILING)
		{
			if (isThreadRunning)
			{
				isThreadRunning = false;
			}
			else
			{
				isThreadRunning = true;
				hTimer = CreateThread(NULL, 0, timerProc, (LPVOID)TIMER_INTERVAL_MS, NULL, NULL);
			}
		}
		else if (id == IDSM_ENABLE_MOUSETOOL)
		{

		}
		else if (id == IDSM_SHOW_CONSOLE)
		{
			bConsolePrint = !IsWindowVisible(hConsoleWnd);
			if (bConsolePrint)
			{
				ShowWindow(hConsoleWnd, SW_SHOW);
			}
			else
			{
				ShowWindow(hConsoleWnd, SW_HIDE);
			}
		}
		else if (id == IDSM_ENABLE_CONSOLE_OUTPUT)
		{
			bConsolePrint = !bConsolePrint;
		}
		else if (id == IDSM_BLOCK_WINDOW)
		{

		}
	}
		break;

	case WM_BLOCK_CALLBACK:
	{
		switch (wparam)
		{
		case BLOCK_CALLBACK_HWND:
			tileManager->addHwndBlock((HWND)lparam);
			break;
		case BLOCK_CALLBACK_CLASS:
			tileManager->addClassBlock((LPSTR)lparam);
			break;
		case BLOCK_CALLBACK_TEXT:
			tileManager->addTextBlock((LPSTR)lparam);
			break;
		}
	}
		break;

	case MSG_TRAYICON:
	{
		if (lparam == WM_LBUTTONDBLCLK)
		{
			ShowWindow(hWnd, SW_SHOW);
		}
		else if (lparam == WM_RBUTTONUP)
		{
			popupMenu(hWnd);
		}
	}
		break;

	case WM_CLOSE:
		ShowWindow(hWnd, SW_HIDE);
		return true;	//防止窗口被销毁
		break;
	case WM_DESTROY:
		delTrayIcon();
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, msg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstace, LPSTR args, int argc)
{
	WNDCLASS wnc;
	MSG msg;

	mhInstance = hInstance;

	wnc.lpfnWndProc = MainWndProc;
	wnc.hInstance = hInstance;
	wnc.hCursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);
	wnc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wnc.lpszMenuName = NULL;
	wnc.cbClsExtra = NULL;
	wnc.hIcon = (HICON)LoadIcon(NULL, IDI_APPLICATION);
	wnc.style = CS_HREDRAW | CS_VREDRAW;
	wnc.cbWndExtra = NULL;
	wnc.lpszClassName = APP_WIN_CLASS_MAIN;
	wnc.lpszMenuName = NULL;
	
	if (!RegisterClass(&wnc))
	{
		MessageBox(0, "Register Window Class Failed.", "ERROR", MB_ICONERROR);
		return -1;
	}

	if (!registerBlkWndClass(mhInstance))
	{
		MessageBox(0, "Register Window Class Failed.", "ERROR", MB_ICONERROR);
		return -1;
	}
	
	hWnd = CreateWindow(APP_WIN_CLASS_MAIN, APP_TITLE_MAIN,
		WS_OVERLAPPEDWINDOW^WS_THICKFRAME^WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	
	if (!hWnd)
	{
		MessageBox(0, "Create Window Failed.", "ERROR", MB_ICONERROR);
		return -2;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;

}

void initCompoents(HWND hParent)
{

	int editboxy = COMPOENT_MARGIN;

	hCBMonitors = CreateWindow("combobox", NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN | CBS_HASSTRINGS | CBS_DROPDOWNLIST,
		COMPOENT_MARGIN, COMPOENT_MARGIN, WIDTH_MONITORLIST, HEIGTH_MONITORLIST,
		hParent, (HMENU)ID_CB_MONITOR, mhInstance, NULL);


	CreateWindow("static", "窗口句柄（HWND）",
		WS_CHILD | WS_VISIBLE,
		COMPOENT_MARGIN * 2 + WIDTH_WINDOWLIST, editboxy,
		WIDTH_EDITBOX, HEIGTH_EDITBOX,
		hParent, NULL, mhInstance, NULL);

	editboxy += COMPOENT_MARGIN + HEIGTH_EDITBOX;

	hETHwnd = CreateWindow("edit", "123123",
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY,
		COMPOENT_MARGIN * 2 + WIDTH_WINDOWLIST, editboxy,
		WIDTH_EDITBOX, HEIGTH_EDITBOX,
		hParent, (HMENU)ID_EB_HWND, mhInstance, NULL);


	hLBWindows = CreateWindow("listbox", NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
		COMPOENT_MARGIN, editboxy,
		WIDTH_WINDOWLIST, HEIGTH_WINDOWLIST,
		hParent, (HMENU)ID_LB_WINDOWS, mhInstance, NULL);

	editboxy += COMPOENT_MARGIN + HEIGTH_EDITBOX;

	CreateWindow("static", "窗口文本（Text）",
		WS_CHILD | WS_VISIBLE,
		COMPOENT_MARGIN * 2 + WIDTH_WINDOWLIST, editboxy,
		WIDTH_EDITBOX, HEIGTH_EDITBOX,
		hParent, NULL, mhInstance, NULL);

	editboxy += COMPOENT_MARGIN + HEIGTH_EDITBOX;

	hETText = CreateWindow("edit", NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY,
		COMPOENT_MARGIN * 2 + WIDTH_WINDOWLIST, editboxy,
		WIDTH_EDITBOX, HEIGTH_EDITBOX,
		hParent, (HMENU)ID_EB_TEXT, mhInstance, NULL);

	editboxy += COMPOENT_MARGIN + HEIGTH_EDITBOX;

	CreateWindow("static", "窗口类名（ClassName）",
		WS_CHILD | WS_VISIBLE,
		COMPOENT_MARGIN * 2 + WIDTH_WINDOWLIST, editboxy,
		WIDTH_EDITBOX, HEIGTH_EDITBOX,
		hParent, NULL, mhInstance, NULL);

	editboxy += COMPOENT_MARGIN + HEIGTH_EDITBOX;

	hETClass = CreateWindow("edit", NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY,
		COMPOENT_MARGIN * 2 + WIDTH_WINDOWLIST, editboxy,
		WIDTH_EDITBOX, HEIGTH_EDITBOX,
		hParent, (HMENU)ID_EB_CLASS, mhInstance, NULL);

	editboxy += COMPOENT_MARGIN + HEIGTH_EDITBOX;

	CreateWindow("static", "窗口位置（x，y）",
		WS_CHILD | WS_VISIBLE,
		COMPOENT_MARGIN * 2 + WIDTH_WINDOWLIST, editboxy,
		WIDTH_EDITBOX, HEIGTH_EDITBOX,
		hParent, NULL, mhInstance, NULL);

	editboxy += COMPOENT_MARGIN + HEIGTH_EDITBOX;

	hETPos = CreateWindow("edit", NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY,
		COMPOENT_MARGIN * 2 + WIDTH_WINDOWLIST, editboxy,
		WIDTH_EDITBOX, HEIGTH_EDITBOX,
		hParent, (HMENU)ID_EB_POS, mhInstance, NULL);

	editboxy += COMPOENT_MARGIN + HEIGTH_EDITBOX;

	CreateWindow("static", "窗口大小（heigth：width）",
		WS_CHILD | WS_VISIBLE,
		COMPOENT_MARGIN * 2 + WIDTH_WINDOWLIST, editboxy,
		WIDTH_EDITBOX, HEIGTH_EDITBOX,
		hParent, NULL, mhInstance, NULL);

	editboxy += COMPOENT_MARGIN + HEIGTH_EDITBOX;

	hETSize = CreateWindow("edit", NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY,
		COMPOENT_MARGIN * 2 + WIDTH_WINDOWLIST, editboxy,
		WIDTH_EDITBOX, HEIGTH_EDITBOX,
		hParent, (HMENU)ID_EB_SIZE, mhInstance, NULL);
}

void updateWindowInfo()
{

	char buf[BUFFER_SIZE];
	int winindex = lbGetSelectItem();
	if (winindex < 0) return;

	list<CWindow>::iterator win = winlist.begin();
	advance(win, winindex);
	
	win->getClassName(buf, BUFFER_SIZE);
	SetWindowText(hETClass, buf);
	win->getText(buf, BUFFER_SIZE);
	SetWindowText(hETText, buf);
	snprintf(buf, BUFFER_SIZE, "%lld", (long long)win->getHandle());
	SetWindowText(hETHwnd, buf);
	POINT p;
	win->getPos(&p);
	snprintf(buf, BUFFER_SIZE, "%d, %d", p.x, p.y);
	SetWindowText(hETPos, buf);
	RECT r;
	win->getRect(&r);
	snprintf(buf, BUFFER_SIZE, "%d : %d", r.bottom - r.top, r.right - r.left);
	SetWindowText(hETSize, buf);
}

void updateWinListBox()
{
	char buf[BUFFER_SIZE];
	while (bPrintThreadLock);	// 在打印窗口信息时，等待，防止段错误。

	tileManager->refreshWinList();

	GetWindowText(hCBMonitors, buf, BUFFER_SIZE);
	for (int i = 0; i < BUFFER_SIZE; i++)
	{
		if (buf[i] == '[')
		{
			buf[i] = 0x00;
			break;
		}
	}

	int monitorindex = atoi(buf);

	lbClearList(hLBWindows);

	list<CWindow>::iterator itr = tileManager->getWinListItr();
	winlist.clear();
	for (; !tileManager->isItrEnd(itr); itr++)
	{
		if (tileManager->screenFromWindow(itr->getHandle()) == monitorindex)
		{
			itr->getText(buf, BUFFER_SIZE);
			lbAddItem(hLBWindows, buf);
			winlist.push_back(*itr);
		}
	}

}

int lbGetSelectItem()
{
	char buf[BUFFER_SIZE];
	int monitorindex;
	LRESULT listmax;
	int winindex;

	GetWindowText(hCBMonitors, buf, BUFFER_SIZE);
	for (int i = 0; i < BUFFER_SIZE; i++)
	{
		if (buf[i] == '[')
		{
			buf[i] = 0x00;
			break;
		}
	}

	monitorindex = atoi(buf);

	listmax = SendMessage(hLBWindows, LB_GETCOUNT, NULL, NULL);
	if (listmax == LB_ERR) return -1;

	for (winindex = 0; winindex < listmax; winindex++)
	{
		LRESULT res = SendMessage(hLBWindows, LB_GETSEL, (WPARAM)winindex, NULL);
		if (res)
		{
			break;
		}
	}

	return winindex;
}

void lbAddItem(HWND hwnd, char* str)
{
	SendMessage(hwnd, LB_ADDSTRING, NULL, (LPARAM)str);
}

void lbClearList(HWND hwnd)
{
	SendMessage(hwnd, LB_RESETCONTENT, NULL, NULL);
}

void cbAddItem(HWND hwnd, char* str)
{
	SendMessage(hwnd, CB_ADDSTRING, NULL, (LPARAM)str);
}

void cbClearList(HWND hwnd)
{
	SendMessage(hwnd, CB_RESETCONTENT, NULL, NULL);
}

void popupMenu(HWND hwnd)
{
	POINT p;
	GetCursorPos(&p);
	SetForegroundWindow(hwnd);
	HMENU pMenu = GetSubMenu(hMenu, 0);

	if (isThreadRunning)
		CheckMenuItem(pMenu, IDSM_ENABLE_TILING, MF_CHECKED);
	else
		CheckMenuItem(pMenu, IDSM_ENABLE_TILING, MF_UNCHECKED);

	if (IsWindowVisible(hConsoleWnd))
		CheckMenuItem(pMenu, IDSM_SHOW_CONSOLE, MF_CHECKED);
	else
		CheckMenuItem(pMenu, IDSM_SHOW_CONSOLE, MF_UNCHECKED);

	if (bConsolePrint)
		CheckMenuItem(pMenu, IDSM_ENABLE_CONSOLE_OUTPUT, MF_CHECKED);
	else
		CheckMenuItem(pMenu, IDSM_ENABLE_CONSOLE_OUTPUT, MF_UNCHECKED);

	int ret = TrackPopupMenu(pMenu, TPM_TOPALIGN, p.x, p.y, NULL, hwnd, NULL);

}

int main()
{
	 SetConsoleTitle(APP_TITLE_CONSOLE);
	 Sleep(50);	//	等待窗口改名完毕
	 hConsoleWnd = FindWindow("ConsoleWindowClass", APP_TITLE_CONSOLE);
	 // 更名后使得在寻找窗口的时候更不容易出错。

	 bConsolePrint = false;
	 ShowWindow(hConsoleWnd, SW_HIDE);

	 HINSTANCE hinstance = GetModuleHandle(0);
	 WinMain(hinstance, 0, NULL, 0);

	return 0;
}

