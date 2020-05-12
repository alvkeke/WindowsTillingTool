#include "common.h"

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

	SetWindowPos(hWnd, HWND_TOPMOST, tx, ty, nWidth + ptDiff.x, nHeight + ptDiff.y, NULL);
}
