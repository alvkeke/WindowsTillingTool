#include "main.h"
#include "TileManager.h"
#include "TrayIcon.h"
#include "resource.h"
#include "BlockSettingWindow.h"
#include "INIHandler.h"



/*++++++++++Main Window Handle++++++++++++++++++++++++*/
HINSTANCE mhInstance;

HWND hConsoleWnd;
HWND hWnd;
HMENU hMenu;

HWND hCBMonitors;
HWND hLBWindows;
HWND hETText;
HWND hETClass;
HWND hETHwnd;
HWND hETPos;
HWND hETSize;

/*++++++++++MouseTool Hook++++++++++++++++++++++++*/
// Hook
HHOOK kHook, mHook;

// 窗口与鼠标信息
HWND fWin;
RECT winRect;
POINT pDown, pNow;

// 当前状态
bool bMouseToolEnabled;
bool bIsFuncKeyDown;
bool bWant2Move;
bool bWant2Size;

bool bIsFuncKeySet;
bool bIsAssistKeyDown;
bool bBlockFuncKey;


/*++++++++++Tiling Hook++++++++++++++++++++++++*/
HWINEVENTHOOK wHook;
bool bAutoTileEnabled;
//bool bPrintThreadLock = false;
bool bConsolePrint;

/*++++++++++其他变量++++++++++++++++++++++++*/
list<CWindow> winlist;
TileManager* tileManager;
INIHandler* iniHandler;


void WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd,
	LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime)
{
	if (!hwnd) return;
	if (idObject != OBJID_WINDOW) return;

	if (event == EVENT_SYSTEM_MOVESIZEEND
		|| event == EVENT_SYSTEM_FOREGROUND
		|| event == EVENT_SYSTEM_MINIMIZEEND
		|| event == EVENT_SYSTEM_DESKTOPSWITCH
		|| event == EVENT_OBJECT_CREATE
		|| event == EVENT_OBJECT_DESTROY
		|| event == EVENT_OBJECT_HIDE
		|| event == EVENT_OBJECT_SHOW
	)
	{

		if (tileManager)
		{

			if (tileManager->refreshWinList())
			{
				updateWinListBox();
			}
			tileManager->tileWindows();
			
			if (bConsolePrint)
			{
				system("cls");
				// bPrintThreadLock = true;	// 线程锁
				tileManager->printWinList();
				// bPrintThreadLock = false;
			}
		}
	}
}

LRESULT CALLBACK KeyHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT kbdata = (PKBDLLHOOKSTRUCT)lParam;

	if (nCode >= 0)
	{
		// 所有win键按下时都屏蔽, 其他按键放行
		if (!bIsFuncKeyDown && kbdata->vkCode == HOOK_KEY_FUNC && (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN))
		{
			bIsFuncKeyDown = true;
			bIsFuncKeySet = false;
			bIsAssistKeyDown = false;
			bWant2Move = false;
			bWant2Size = false;
			bBlockFuncKey = false;
			return 1;
		}
		else if (bIsFuncKeyDown)
		{
			if (kbdata->vkCode == HOOK_KEY_FUNC)
			{
				if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
				{

					if (!bIsFuncKeySet && !bBlockFuncKey)
					{
						keybd_event(HOOK_KEY_FUNC, 0, 0, 0);
						keybd_event(HOOK_KEY_FUNC, 0, KEYEVENTF_KEYUP, 0);
					}
					else if (bIsFuncKeySet)
					{
						keybd_event(HOOK_KEY_FUNC, 0, KEYEVENTF_KEYUP, 0);
						bIsFuncKeySet = false;
					}

					bIsAssistKeyDown = false;
					bIsFuncKeyDown = false;
				}
			}
			else if (kbdata->vkCode == HOOK_KEY_ASSIST)
			{
				bBlockFuncKey = true;
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
				{
					bIsAssistKeyDown = true;
				}
				else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
				{
					bIsAssistKeyDown = false;
				}
			}
			else if (kbdata->vkCode == HOOK_KEY_MOUSETOOL_SWITCH)
			{
				bBlockFuncKey = true;
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
				{
					
					//SendMessage(hWnd, WM_CALLBACK_SWITCH, 0, 0);
					if (bMouseToolEnabled)
						disableMouseTool();
					else
						enableMouseTool();
				}
			}
			else if (kbdata->vkCode == HOOK_KEY_TILE_SWITCH)
			{
				bBlockFuncKey = true;
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
				{
					
					if (bAutoTileEnabled)
						disableTiling();
					else
						enableTiling();
				}
			}
			else if (kbdata->vkCode == HOOK_KEY_FULLWINDOW)
			{
				bBlockFuncKey = true;
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
				{
					tileManager->toggleWinTmpFull(GetForegroundWindow());
				}
			}
			else if (bIsAssistKeyDown && kbdata->vkCode == HOOK_KEY_FLOATWINDOW)
			{
				bBlockFuncKey = true;
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
				{
					tileManager->toggleWinFloat(GetForegroundWindow());
				}
			}
			else if (kbdata->vkCode == HOOK_KEY_UP)
			{
				bBlockFuncKey = true;
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
				{
					if (bIsAssistKeyDown)
					{
					}
					else
					{
					}
				}
			}
			else if (kbdata->vkCode == HOOK_KEY_DOWN)
			{
				bBlockFuncKey = true;
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
				{
					if (bIsAssistKeyDown)
					{
					}
					else
					{
					}
				}
			}
			else if (kbdata->vkCode == HOOK_KEY_LEFT)
			{
				bBlockFuncKey = true;

				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
				{
					if (bIsAssistKeyDown)
					{
						tileManager->moveFocusWindowLeft();
						tileManager->tileWindows();
					}
					else
					{
					}
				}

			}
			else if (kbdata->vkCode == HOOK_KEY_RIGHT)
			{
				bBlockFuncKey = true;

				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
				{
					if (bIsAssistKeyDown)
					{
						tileManager->moveFocusWindowRight();
						tileManager->tileWindows();
					}
					else
					{
					}
				}
			}
			else
			{
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
				{
					// 只有在第一次按下时设置win键被按下

					bIsFuncKeySet = true;
					keybd_event(HOOK_KEY_FUNC, 0, 0, 0);
					keybd_event(kbdata->vkCode, 0, 0, 0);
				}
				else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
				{
					keybd_event(kbdata->vkCode, 0, KEYEVENTF_KEYUP, 0);
				}
			}

			return 1;
		}
	}

	return CallNextHookEx(kHook, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	LPMSLLHOOKSTRUCT mdata = (LPMSLLHOOKSTRUCT)lParam;
	POINT   pt = mdata->pt;

	if (bMouseToolEnabled && nCode >= 0 && bIsFuncKeyDown)
	{
		int newX, newY;
		int newW, newH;
		GetCursorPos(&pNow);
		newX = winRect.left + pNow.x - pDown.x;
		newY = winRect.top + pNow.y - pDown.y;

		newW = winRect.right - winRect.left + pNow.x - pDown.x;
		newH = winRect.bottom - winRect.top + pNow.y - pDown.y;

		if (wParam == WM_MBUTTONUP)
		{
			bBlockFuncKey = true;

			GetCursorPos(&pDown);
			fWin = WindowFromPoint(pDown);
			if (fWin == 0) goto exitl;
			while (GetParent(fWin)) {
				fWin = GetParent(fWin);
			}
			if (IsZoomed(fWin))
			{
				ShowWindow(fWin, SW_SHOWNORMAL);
			}
			else
			{
				ShowWindow(fWin, SW_MAXIMIZE);
			}

			return 1;
		}
		else if (wParam == WM_MBUTTONDOWN)
		{
			// 屏蔽中键按下事件，防止中键被保持按下状态。
			return 1;
		}
		else if (wParam == WM_MOUSEWHEEL)
		{
			bBlockFuncKey = true;

			GetCursorPos(&pDown);
			fWin = WindowFromPoint(pDown);
			if (fWin == 0) goto exitl;
			while (GetParent(fWin)) {
				fWin = GetParent(fWin);
			}
			SetFocus(fWin);

			short delta = GET_WHEEL_DELTA_WPARAM(mdata->mouseData);
			if (delta > 0)
			{
				SetWindowPos(fWin, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			else
			{
				SetWindowPos(fWin, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			return 1;
		}
		else if (bWant2Move && wParam == WM_MOUSEMOVE)
		{
			if (IsZoomed(fWin)) {
				ShowWindow(fWin, SW_SHOWNORMAL);
			}
			SetWindowPos(fWin, 0, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

		}
		else if (bWant2Size && wParam == WM_MOUSEMOVE)
		{
			if (IsZoomed(fWin)) {
				ShowWindow(fWin, SW_SHOWNORMAL);
			}
			SetWindowPos(fWin, 0, 0, 0, newW, newH, SWP_NOMOVE | SWP_NOZORDER);
		}
		else if (wParam == WM_LBUTTONDOWN)
		{
			if (!bWant2Size)
			{
				bBlockFuncKey = true;

				GetCursorPos(&pDown);
				fWin = WindowFromPoint(pDown);
				if (fWin == 0) goto exitl;

				while (!tileManager->isWinInList(fWin))
				{
					fWin = GetParent(fWin);
				}

				SetForegroundWindow(fWin);

				char pname[256];
				RealGetWindowClass(fWin, pname, 255);

				GetWindowRect(fWin, &winRect);
				bWant2Move = true;
				if (!strcmp(pname, "Windows.UI.Core.CoreWindow")) bWant2Move = false;

			}

			return 1;
		}
		else if (wParam == WM_RBUTTONDOWN)
		{
			if (!bWant2Move)
			{
				bBlockFuncKey = true;

				GetCursorPos(&pDown);
				fWin = WindowFromPoint(pDown);
				if (fWin == 0) goto exitl;

				while(!tileManager->isWinInList(fWin))
				{
					fWin = GetParent(fWin);
				}

				SetForegroundWindow(fWin);

				char pname[256];
				RealGetWindowClass(fWin, pname, 255);
				//for (int i = 0; i < MarkedClasses.size(); i++) {
				//	if (MarkedClasses.at(i)._Equal(pname)) goto exitl;
				//}

				GetWindowRect(fWin, &winRect);
				bWant2Size = true;
				if (!strcmp(pname, "Windows.UI.Core.CoreWindow")) bWant2Size = false;
			}

			return 1;
		}
		else if (bWant2Move && wParam == WM_LBUTTONUP)
		{
			// SetWindowPos(fWin, 0, newX, newY, 0, 0, SWP_NOSIZE);
			SendMessage(fWin, WM_MOVE, 0, (LPARAM)MAKELONG(newX, newY));
			bWant2Move = false;
			return 1;
		}
		else if (bWant2Size && wParam == WM_RBUTTONUP)
		{
			//SetWindowPos(fWin, 0, 0, 0, newW, newH, SWP_NOMOVE);
			SendMessage(fWin, WM_SIZE, 0, (LPARAM)MAKELONG(newW, newH));
			bWant2Size = false;
			return 1;
		}

	}

exitl:
	return CallNextHookEx(mHook, nCode, wParam, lParam);
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	switch (msg)
	{
	case WM_CREATE:
	{
		char buf[BUFFER_SIZE];

		//bPrintThreadLock = false;

		hMenu = LoadMenu(mhInstance, MAKEINTRESOURCE(IDR_MENU1));

		initCompoents(hWnd);
		initTrayIcon(mhInstance, hWnd);
		ReadjustWindow(hWnd, 560, 470);

		tileManager = new TileManager(hWnd);
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

		loadConfiguration();

		enableTiling();
		// enableMouseTool();	//测试时关闭
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
			if (HIWORD(wparam) == CBN_SELCHANGE)
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
			terminateApplication();
		}
		else if (id == IDSM_RELOADDATA)
		{
			iniHandler->clearAllData();
			tileManager->clearAllBlock();
			loadConfiguration();
			tileManager->tileWindows();
		}
		else if (id == IDSM_ENABLE_TILING)
		{
			if (bAutoTileEnabled)
			{
				disableTiling();
			}
			else
			{
				enableTiling();
			}
		}
		else if (id == IDSM_ENABLE_MOUSETOOL)
		{
			if (bMouseToolEnabled)
				disableMouseTool();
			else
				enableMouseTool();
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
			tileAddHwndBlock((HWND)lparam);
			break;
		case BLOCK_CALLBACK_CLASS:
			tileAddClassBlock(*new string((LPSTR)lparam));
			break;
		case BLOCK_CALLBACK_TEXT:
			tileAddTextBlock(*new string((LPSTR)lparam));
			break;
		}
	}
		break;

	case WM_BLOCK_BOTH_CALLBACK:
		tileAddBothBlock(*new string((LPSTR)wparam), *new string((LPSTR)lparam));
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
		terminateApplication();
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
	wnc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
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

	int ret = initHook(hInstance);
	if (ret == 1)
	{
		MessageBox(0, "Create Key Hook Failed.", "ERROR", MB_ICONERROR);
		return -3;
	}
	else if (ret == 2)
	{
		MessageBox(0, "Create Mouse Hook Failed.", "ERROR", MB_ICONERROR);
		return -3;
	}

	//ShowWindow(hWnd, SW_SHOW);
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
	//while (bPrintThreadLock);	// 在打印窗口信息时，等待，防止段错误。

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

	if(bMouseToolEnabled)
		CheckMenuItem(pMenu, IDSM_ENABLE_MOUSETOOL, MF_CHECKED);
	else
		CheckMenuItem(pMenu, IDSM_ENABLE_MOUSETOOL, MF_UNCHECKED);

	if (bAutoTileEnabled)
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

void loadConfiguration()
{
	iniHandler = new INIHandler();
	iniHandler->praseFile(INI_FILE_NAME);

	INISection* sec;
	sec = iniHandler->getSection(SECNAME_TILE_SETTING);
	if (sec)
	{
		string spx = sec->getValue(SC_TILE_PADDINGX);
		string spy = sec->getValue(SC_TILE_PADDINGY);
		int px = -1;
		int py = -1;
		if (!spx.empty()) px = atoi(spx.c_str());
		if (!spy.empty()) py = atoi(spy.c_str());

		// -1为不更改
		tileManager->setTilePadding(px, py);
	}

	sec = iniHandler->getSection(SECNAME_CLASS_BLOCK_LIST);
	if(sec)
	{
		ITEMITR itr = sec->getItemItr();
		for (; !sec->isItemItrEnd(itr); itr++)
		{
			tileManager->addClassBlock(itr->getValue());
		}
	}

	sec = iniHandler->getSection(SECNAME_TEXT_BLOCK_LIST);
	if (sec)
	{
		ITEMITR itr = sec->getItemItr();
		for (; !sec->isItemItrEnd(itr); itr++)
		{
			tileManager->addTextBlock(itr->getValue());
		}
	}

	sec = iniHandler->getSection(SECNAME_TEXT_PART_BLOCK_LIST);
	if (sec)
	{
		ITEMITR itr = sec->getItemItr();
		for (; !sec->isItemItrEnd(itr); itr++)
		{
			tileManager->addTextPartBlock(itr->getValue());
		}
	}

	sec = iniHandler->getSection(SECNAME_CLASS_PART_BLOCK_LIST);
	if (sec)
	{
		ITEMITR itr = sec->getItemItr();
		for (; !sec->isItemItrEnd(itr); itr++)
		{
			tileManager->addClassPartBlock(itr->getValue());
		}
	}

	sec = iniHandler->getSection(SECNAME_BOTH_BLOCK_LIST);
	if (sec)
	{
		ITEMITR itr = sec->getItemItr();
		for (; !sec->isItemItrEnd(itr); itr++)
		{
			// key: classname;  value: name
			tileManager->addBothBlock(itr->getKey(), itr->getValue());
		}
	}

}

void tileAddHwndBlock(HWND hwnd)
{			
	tileManager->addHwndBlock(hwnd);
}

void tileAddClassBlock(string classname)
{
	tileManager->addClassBlock(classname);
	iniHandler->addItem(SECNAME_CLASS_BLOCK_LIST, "none", classname);
	iniHandler->overwiteFile(INI_FILE_NAME);
}

void tileAddTextBlock(string text)
{		
	tileManager->addTextBlock(text);
	iniHandler->addItem(SECNAME_TEXT_BLOCK_LIST, "none", text);
	iniHandler->overwiteFile(INI_FILE_NAME);
}

void tileAddBothBlock(string classname, string text)
{
	tileManager->addBothBlock(classname, text);
	 iniHandler->addItem(SECNAME_BOTH_BLOCK_LIST, classname, text);
	 iniHandler->overwiteFile(INI_FILE_NAME);
}

void enableTiling()
{
	// todo: 更改事件响应范围
	wHook = SetWinEventHook(EVENT_MIN, EVENT_MAX, NULL,
		WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
	if (wHook) bAutoTileEnabled = true;
}

void disableTiling()
{
	if (UnhookWinEvent(wHook)) bAutoTileEnabled = false;
}

int initHook(HINSTANCE hInstance)//, HWND hWnd)
{
	bMouseToolEnabled = false;
	bIsFuncKeyDown = false;
	bWant2Move = false;
	bWant2Size = false;
	bIsFuncKeySet = false;
	bBlockFuncKey = false;

	kHook = SetWindowsHookEx(
		WH_KEYBOARD_LL,
		KeyHookProc,
		hInstance,
		0
	);

	mHook = SetWindowsHookEx(
		WH_MOUSE_LL,
		MouseHookProc,
		hInstance,
		0
	);

	if (kHook == NULL)
	{
		return 1;
	}

	if (mHook == NULL)
	{
		return 2;
	}

	disableMouseTool();

	return 0;
}

void enableMouseTool()
{
	bMouseToolEnabled = true;
}

void disableMouseTool()
{
	bMouseToolEnabled = false;
}

void terminateApplication()
{
	delTrayIcon();
	UnhookWinEvent(wHook);
	UnhookWindowsHookEx(kHook);
	UnhookWindowsHookEx(mHook);
	PostQuitMessage(0);
}

int main()
{

	 SetConsoleTitle(APP_TITLE_CONSOLE);
	 Sleep(50);	//	等待窗口改名完毕
	 hConsoleWnd = FindWindow("ConsoleWindowClass", APP_TITLE_CONSOLE);
	 // 更名后使得在寻找窗口的时候更不容易出错。

	 ShowWindow(hConsoleWnd, SW_HIDE);
	 bConsolePrint = false;

	 HINSTANCE hinstance = GetModuleHandle(0);
	 WinMain(hinstance, 0, NULL, 0);

	 //list<int> l1;
	 //for (int i = 0; i < 10; i++)
	 //{
		// l1.push_back(i);
	 //}

	 //list<int>::iterator itr, itr2;
	 //for (itr = l1.begin(); itr != l1.end(); itr++)
	 //{
		// cout << *itr << endl;
	 //}

	 //itr = l1.begin();
	 //itr2 = l1.begin();
	 //advance(itr, 4);
	 //advance(itr2, 5);
	 //swap(*itr, *itr2);

	 //cout << endl << endl;
	 //for (itr = l1.begin(); itr != l1.end(); itr++)
	 //{
		// cout << *itr << endl;
	 //}



	return 0;
}

