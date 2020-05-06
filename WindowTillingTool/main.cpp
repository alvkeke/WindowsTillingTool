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
WindowManager* windows;

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	switch (msg)
	{
	case WM_CREATE:
	{
		char buf[50];
		initCompoents(hWnd);
		ReadjustWindow(hWnd, 560, 470);

		windows = new WindowManager();
		monitors = windows->getMonitormanager();

		for (int i = 0; i < monitors->getMonitorCount(); i++)
		{
			MonitorNode* m = monitors->getMonitor(i);
			snprintf(buf,50,  "%d [%d x %d]\0", i, m->getRight()-m->getLeft(), m->getBottom()-m->getTop());
			cbAddItem(hCBMonitors, buf);
			if (i == 0)
			{
				// SendMessage(hCBMonitors, CB_SELECTSTRING, 0, (WPARAM)buf);
			}
		}
	}

		break;
	case WM_COMMAND:
	{
		int id = LOWORD(wparam);
		if (id == ID_CB_MONITOR)
		{
			if (HIWORD(wparam) == CBN_SELCHANGE)
			{
				lbClearList(hLBWindows);
				char buf[50];
					
				GetWindowText(hCBMonitors, buf, 50);
				for (int i = 0; i < 50; i++)
				{
					if (buf[i] == '[')
					{
						buf[i] = 0x00;
						break;
					}
				}

				int monitorindex = atoi(buf);
				WindowNode* windowlist = windows->getWindowList(monitorindex);

				for (WindowNode* itr = windowlist; itr != NULL; itr = itr->mNext)
				{
					itr->getText(buf, 50);
					lbAddItem(hLBWindows, buf);
				}
			}
		}
		else if (id == ID_LB_WINDOWS)
		{

			if (HIWORD(wparam) == LBN_SELCHANGE)
			{
				char buf[50];
				int monitorindex;
				int listmax;
				int winindex;

				GetWindowText(hCBMonitors, buf, 50);
				for (int i = 0; i < 50; i++)
				{
					if (buf[i] == '[')
					{
						buf[i] = 0x00;
						break;
					}
				}

				monitorindex = atoi(buf);
				
				listmax = SendMessage(hLBWindows, LB_GETCOUNT, NULL, NULL);
				if (listmax == LB_ERR) break;
				
				for (winindex = 0; winindex < listmax; winindex++)
				{
					int res = SendMessage(hLBWindows, LB_GETSEL, (WPARAM)winindex, NULL);
					if (res) 
					{
						break;
					}
				}

				WindowNode* win = windows->getWindowNode(monitorindex, winindex);
				win->getClassName(buf, 50);
				SetWindowText(hETClass, buf);
				win->getText(buf, 50);
				SetWindowText(hETText, buf);
				snprintf(buf, 50, "%d", win->getHandle());
				SetWindowText(hETHwnd, buf);
				POINT p;
				win->getPos(&p);
				snprintf(buf, 50, "%d, %d", p.x, p.y);
				SetWindowText(hETPos, buf);
				RECT r;
				win->getRect(&r);
				snprintf(buf, 50, "%d : %d", r.bottom-r.top, r.right-r.left);
				SetWindowText(hETSize, buf);
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
		MessageBox(0, "ERROR1", "ERROR", 0);
		return -1;
	}
	
	hWnd = CreateWindow(MAINWND_CLASS_NAME, "WinTool",
		WS_OVERLAPPEDWINDOW^WS_THICKFRAME^WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	
	if (!hWnd)
	{
		MessageBox(0, "ERROR2", "ERROR", 0);
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

void ReadjustWindow(HWND hWnd, int nWidth, int nHeight)
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

//int main()
//{
//	WindowManager* w = new WindowManager();
//	w->printWindowList();
//	cout << endl << endl;
//	cout << endl << endl;
//	w->clearWindows();
//	w->printWindowList();
//	cout << endl << endl;
//	cout << endl << endl;
//	w->refreshWindowList();
//	w->printWindowList();
//	return 0;
//}
