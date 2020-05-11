#include "main.h"

HINSTANCE mhInstance;

HWND hWnd;

HWND hCBMonitors;
HWND hLBWindows;
HWND hETText;
HWND hETClass;
HWND hETHwnd;
HWND hETPos;
HWND hETSize;

MonitorManager* monitors;
WindowsManager* windows;

list<CWindow> winlist;

bool isPrinting = false;

DWORD WINAPI timerProc(LPVOID interval)
{
	while (true)
	{
		if (windows)
		{
			windows->refreshWindowList();
			// AdjustWindows();
			// updateWindowsList();
			// updateWindowInfo();
			isPrinting = true;	// 线程锁
			windows->printWindowList();
			isPrinting = false;
		}
		Sleep((int)interval);
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
		winlist.clear();

		initCompoents(hWnd);
		ReadjustMainWindow(hWnd, 560, 470);

		windows = new WindowsManager();
		monitors = new MonitorManager();

		for (int i = 0; i < monitors->getMonitorCount(); i++)
		{
			CMonitor* m = monitors->getMonitor(i);
			snprintf(buf, BUFFER_SIZE,  "%d [%d x %d]\0", i, m->getRight()-m->getLeft(), m->getBottom()-m->getTop());
			cbAddItem(hCBMonitors, buf);
			if (i == 0)
			{
				SendMessage(hCBMonitors, CB_SELECTSTRING, 0, (WPARAM)buf);
				updateWindowsList();
			}
		}

		CreateThread(NULL, 0, timerProc, (LPVOID)TIMER_INTERVAL_MS, NULL, NULL);
	}
		break;
	case WM_RBUTTONUP:
	{
		// AdjustWindows();
		// MessageBox(0, "", "", 0);
	}
		break;
	case WM_COMMAND:
	{
		int id = LOWORD(wparam);
		if (id == ID_CB_MONITOR)
		{
			// if (HIWORD(wparam) == CBN_SELCHANGE)
			{
				updateWindowsList();
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

	MoveWindow(hWnd, tx, ty, nWidth + ptDiff.x, nHeight + ptDiff.y, TRUE);
}

void updateWindowInfo()
{
	char buf[BUFFER_SIZE];
	int monitorindex;
	int listmax;
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
		int res = SendMessage(hLBWindows, LB_GETSEL, (WPARAM)winindex, NULL);
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
	snprintf(buf, BUFFER_SIZE, "%d", win->getHandle());
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

void updateWindowsList()
{
	char buf[BUFFER_SIZE];
	//if (isPrinting) return;
	while (isPrinting);	// 在打印窗口信息时，等待，防止段错误。

	windows->refreshWindowList();

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

	list<CWindow>::iterator itr = windows->getItrBegin();
	winlist.clear();
	for (; !windows->isItrEnd(itr); itr++)
	{
		if (monitors->monitorFromWindow(itr->getHandle()) == monitorindex)
		{
			itr->getText(buf, BUFFER_SIZE);
			lbAddItem(hLBWindows, buf);
			winlist.push_back(*itr);
		}
	}

}

void AdjustWindows()
{

	int n_monitor = monitors->getMonitorCount();

	int* n_window = new int[n_monitor];
	int* winw = new int[n_monitor];
	int* winh = new int[n_monitor];
	int* biasy = new int[n_monitor];
	int* biasx = new int[n_monitor];
	int* add = new int[n_monitor];

	for (int i = 0; i < n_monitor; i++)
	{
		n_window[i] = 0;
		// 获取每个屏幕的起点。
		biasy[i] = monitors->getMonitor(i)->getTop() + WINDOW_MARGIN;
		biasx[i] = monitors->getMonitor(i)->getLeft() + WINDOW_MARGIN;
	}

	list<CWindow>::iterator itr;
	for (itr = windows->getItrBegin(); !windows->isItrEnd(itr); itr++)
	{
		// 获取每个显示器中窗口的个数
		int n = monitors->monitorFromWindow(itr->getHandle());
		if (n < 0 || n >= n_monitor) continue;	// 数据与已有数据不符，则跳过
		// if (n>=0 && n<n_monitor) 
		n_window[n]++;
	}

	for (int i = 0; i < n_monitor; i++)
	{
		// 如果某个显示器下的窗口数量为0，则跳过该显示器。
		if (n_window[i] == 0) continue;
		winh[i] = monitors->getHeight(i);
		winw[i] = monitors->getWidth(i);
		add[i] = (winw[i]-WINDOW_MARGIN)/n_window[i];
		winh[i] -= WINDOW_MARGIN * 2;
		winw[i] = add[i] - WINDOW_MARGIN;
	}

	// 根据已有数据，调整窗口大小与位置。
	for (itr = windows->getItrBegin(); !windows->isItrEnd(itr); itr++)
	{
		// n_window[monitors->monitorFromWindow(itr->getHandle())]++;
		HWND hw = itr->getHandle();
		int n = monitors->monitorFromWindow(hw);
		if (n<0 || n >= n_monitor) continue;	// 数据与已有数据不符，则跳过
		if (n_window[n] == 0) continue;
		ShowWindow(hw, SW_NORMAL);
		MoveWindow(hw, biasx[n], biasy[n], winw[n], winh[n], true);
		biasx[n] += add[n];
	}

	return;

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

typedef list<CWindow>::iterator CWINITR;


// 测试用主函数，正式编译时请将此主函数删除哦，并修改工程属性为窗口
int main()
{

	HINSTANCE hinstance = GetModuleHandle(0);
	WinMain(hinstance, 0, NULL, 0);


	//list<CWindow> list1;
	//for (int i = 1; i < 10; i++)
	//{
	//	list1.push_back(*new CWindow((HWND)i));
	//}

	//CWINITR itr;
	//for (itr = list1.begin(); itr != list1.end(); itr++)
	//{
	//	cout << itr->getHandle() << endl;
	//}

	//for (itr = list1.begin(); itr != list1.end(); itr++)
	//{
	//	// cout << itr->getHandle() << endl;
	//	if (itr->getHandle() == (HWND)5)
	//	{
	//		itr = list1.erase(itr);
	//		// break;
	//	}
	//}

	//
	//cout << endl << endl << endl;

	//for (itr = list1.begin(); itr != list1.end(); itr++)
	//{
	//	cout << itr->getHandle() << endl;
	//}


	return 0;
}

