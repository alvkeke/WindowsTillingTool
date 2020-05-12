#include "main.h"
#include "TileManager.h"

HINSTANCE mhInstance;

HWND hWnd;

HWND hCBMonitors;
HWND hLBWindows;
HWND hETText;
HWND hETClass;
HWND hETHwnd;
HWND hETPos;
HWND hETSize;

list<CWindow> winlist;
TileManager* tileManager;

bool isPrinting = false;

DWORD WINAPI timerProc(LPVOID interval)
{
	while (true)
	{
		if (tileManager)
		{
			tileManager->refreshWinList();
			tileManager->tileWindows();
			isPrinting = true;	// 线程锁
			tileManager->printWinList();
			isPrinting = false;
		}
		Sleep((DWORD)interval);
		system("cls");
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

		isPrinting = false;

		initCompoents(hWnd);
		ReadjustMainWindow(hWnd, 560, 470);

		tileManager = new TileManager(hWnd);

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

		CreateThread(NULL, 0, timerProc, (LPVOID)TIMER_INTERVAL_MS, NULL, NULL);
	}
		break;
	case WM_RBUTTONUP:
	{
		// TileWindows();
	}
		break;
	case WM_COMMAND:
	{
		int id = LOWORD(wparam);
		if (id == ID_CB_MONITOR)
		{
			// if (HIWORD(wparam) == CBN_SELCHANGE)
			{
				updateWinListBox();
			}
		}
		else if (id == ID_LB_WINDOWS)
		{
			if (HIWORD(wparam) == LBN_SELCHANGE)
			{
				updateWindowInfo();
			}
		}
	}

		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_DESTROY:
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
	wnc.lpszClassName = "WinTool_Main";
	wnc.lpszMenuName = NULL;
	
	if (!RegisterClass(&wnc))
	{
		MessageBox(0, "Register Window Class Failed.", "ERROR", MB_ICONERROR);
		return -1;
	}
	
	hWnd = CreateWindow(MAINWND_CLASS_NAME, "WinTool",
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

void ReadjustMainWindow(HWND hWnd, int nWidth, int nHeight)
{
	RECT rcClient, rcWind;
	POINT ptDiff;
	int tx, ty;

	GetClientRect(hWnd, &rcClient);
	GetWindowRect(hWnd, &rcWind);
	ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
	ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;

	tx = GetSystemMetrics(SM_CXSCREEN);
	ty = GetSystemMetrics(SM_CYSCREEN);

	tx = (tx - nWidth - ptDiff.x) / 2;
	ty = (ty - nHeight - ptDiff.y) / 2;

	SetWindowPos(hWnd, HWND_TOPMOST, tx, ty, nWidth+ptDiff.x, nHeight+ptDiff.y, NULL);
}

void updateWindowInfo()
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
	if (listmax == LB_ERR) return;

	for (winindex = 0; winindex < listmax; winindex++)
	{
		LRESULT res = SendMessage(hLBWindows, LB_GETSEL, (WPARAM)winindex, NULL);
		if (res)
		{
			break;
		}
	}

	list<CWindow>::iterator win;
	for (win = winlist.begin(); win != winlist.end(); win++)
	{
		if (winindex-- == 0)break;
	}
	
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
	while (isPrinting);	// 在打印窗口信息时，等待，防止段错误。

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

// 测试用主函数，正式编译时请将此主函数删除哦，并修改工程属性为窗口
int main()
{

	 HINSTANCE hinstance = GetModuleHandle(0);
	 WinMain(hinstance, 0, NULL, 0);

	//WindowsManager* w = new WindowsManager();
	//while (1)
	//{
	//	system("cls");
	//	w->refreshWindowList();
	//	w->printWindowList();
	//	Sleep(1000);
	//}

	return 0;
}
