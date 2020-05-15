#include "BlockSettingWindow.h"

HWND hParent;

HWND hTextClass;
HWND hTextText;
HWND hTextHwnd;


char classbuf[BUFFER_SIZE];
char textbuf[BUFFER_SIZE];
HWND hwndsave;

LRESULT CALLBACK BlkSetWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	switch (msg)
	{
	case WM_CREATE:
	{

	}
	break;

	case WM_CLOSE:
	{
	}
	break;

	case WM_COMMAND:
	{
		switch (LOWORD(wparam))
		{
		//case ID_BTN_CANCEL:
		//	SendMessage(hwnd, WM_CLOSE, 0, 0);
		//	break;
		case ID_BTN_BLOCK_ONCE:
			SendMessage(hParent, WM_BLOCK_CALLBACK, BLOCK_CALLBACK_HWND, (LPARAM)hwndsave);
			break;
		case ID_BTN_BLOCK_CLASS:
			SendMessage(hParent, WM_BLOCK_CALLBACK, BLOCK_CALLBACK_CLASS, (LPARAM)classbuf);
			break;
		case ID_BTN_BLOCK_TEXT:
			SendMessage(hParent, WM_BLOCK_CALLBACK, BLOCK_CALLBACK_TEXT, (LPARAM)textbuf);
			break;
		case ID_BTN_BLOCK_ALL:
			SendMessage(hParent, WM_BLOCK_BOTH_CALLBACK, (WPARAM)classbuf, (LPARAM)textbuf);
			break;
		}
		// 只要按下按钮都会关闭窗口
		SendMessage(hwnd, WM_CLOSE, 0, 0);
	}
	break;

	case WM_BLOCK_SETSTRINFO:
	{
		strcpy(classbuf, (LPSTR)wparam);
		strcpy(textbuf, (LPSTR)lparam);
		SetWindowText(hTextClass, classbuf);
		SetWindowText(hTextText, textbuf);
	}
	break;

	case WM_BLOCK_SETHWND:
	{
		hwndsave = (HWND)wparam;
		char buf[BUFFER_SIZE];
		_itoa((int)wparam, buf, 16);
		SetWindowText(hTextHwnd, buf);
	}
	break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

int registerBlkWndClass(HINSTANCE hInstance)
{
	WNDCLASS wnc;
	wnc.cbClsExtra = sizeof(wnc);
	wnc.cbClsExtra = NULL;
	wnc.cbWndExtra = NULL;
	wnc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wnc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnc.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
	wnc.hInstance = hInstance;
	wnc.lpfnWndProc = BlkSetWndProc;
	wnc.lpszClassName = APP_WIN_CLASS_BLOCK;
	wnc.lpszMenuName = NULL;
	wnc.style = CS_HREDRAW | CS_VREDRAW;

	return RegisterClass(&wnc);
}

HWND createBlkSetWnd(HINSTANCE hinstance, HWND hparent)
{

	hParent = hparent;

	HWND hwnd = CreateWindow(APP_WIN_CLASS_BLOCK, "屏蔽设置",
					(WS_OVERLAPPEDWINDOW &~WS_MAXIMIZEBOX),
					100, 100, 100, 100,
					hparent, NULL, hinstance, NULL);

	CreateWindow("static", TIP_TEXT,
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		COMPOENT_DISTANCE, COMPOENT_DISTANCE,
		TEXT_WIDTH, TEXT_HIGHT,
		hwnd, NULL, hinstance, NULL);

	hTextClass = CreateWindow("static", NULL,
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		COMPOENT_DISTANCE, COMPOENT_DISTANCE+(COMPOENT_DISTANCE+TEXT_HIGHT)*1,
		TEXT_WIDTH, TEXT_HIGHT,
		hwnd, NULL, hinstance, NULL);

	hTextText = CreateWindow("static", NULL,
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		COMPOENT_DISTANCE, COMPOENT_DISTANCE + (COMPOENT_DISTANCE + TEXT_HIGHT)*2,
		TEXT_WIDTH, TEXT_HIGHT,
		hwnd, NULL, hinstance, NULL);

	hTextHwnd = CreateWindow("static", NULL,
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		COMPOENT_DISTANCE, COMPOENT_DISTANCE + (COMPOENT_DISTANCE + TEXT_HIGHT)*3,
		TEXT_WIDTH, TEXT_HIGHT,
		hwnd, NULL, hinstance, NULL);

	HWND hBtnOnce = CreateWindow("button", "只本次",
		WS_CHILD | WS_VISIBLE,
		COMPOENT_DISTANCE, 
		COMPOENT_DISTANCE + (COMPOENT_DISTANCE + TEXT_HIGHT)*4,
		BUTTON_WIDTH, BUTTON_HIGHT,
		hwnd, (HMENU)ID_BTN_BLOCK_ONCE, hinstance, NULL);

	HWND hBtnText = CreateWindow("button", "通过标题",
		WS_CHILD | WS_VISIBLE,
		COMPOENT_DISTANCE + (COMPOENT_DISTANCE + BUTTON_WIDTH) * 1,
		COMPOENT_DISTANCE + (COMPOENT_DISTANCE + TEXT_HIGHT) * 4,
		BUTTON_WIDTH, BUTTON_HIGHT,
		hwnd, (HMENU)ID_BTN_BLOCK_TEXT, hinstance, NULL);

	HWND hBtnClass = CreateWindow("button", "通过类名",
		WS_CHILD | WS_VISIBLE,
		COMPOENT_DISTANCE + (COMPOENT_DISTANCE + BUTTON_WIDTH) * 2,
		COMPOENT_DISTANCE + (COMPOENT_DISTANCE + TEXT_HIGHT) * 4,
		BUTTON_WIDTH, BUTTON_HIGHT,
		hwnd, (HMENU)ID_BTN_BLOCK_CLASS, hinstance, NULL);

	HWND hBtnAll = CreateWindow("button", "类名与标题",
		WS_CHILD | WS_VISIBLE,
		COMPOENT_DISTANCE + (COMPOENT_DISTANCE + BUTTON_WIDTH) * 3,
		COMPOENT_DISTANCE + (COMPOENT_DISTANCE + TEXT_HIGHT) * 4,
		BUTTON_WIDTH, BUTTON_HIGHT,
		hwnd, (HMENU)ID_BTN_BLOCK_ALL, hinstance, NULL);

	HWND hBtnCancel = CreateWindow("button", "取消",
		WS_CHILD | WS_VISIBLE,
		COMPOENT_DISTANCE+(COMPOENT_DISTANCE+BUTTON_WIDTH)*4, 
		COMPOENT_DISTANCE + (COMPOENT_DISTANCE + TEXT_HIGHT) * 4,
		BUTTON_WIDTH, BUTTON_HIGHT,
		hwnd, (HMENU)ID_BTN_CANCEL, hinstance, NULL);

	ReadjustWindow(hwnd, 
		TEXT_WIDTH+2*COMPOENT_DISTANCE, 
		COMPOENT_DISTANCE+4*(COMPOENT_DISTANCE+TEXT_HIGHT)+(COMPOENT_DISTANCE+BUTTON_HIGHT));

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	return hwnd;
}
