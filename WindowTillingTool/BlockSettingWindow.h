#pragma once
#include <Windows.h>
#include "common.h"

#define TIP_TEXT ""
#define ID_BTN_BLOCK_ONCE	20001
#define ID_BTN_BLOCK_TEXT	20002
#define ID_BTN_BLOCK_CLASS	20003
#define ID_BTN_BLOCK_ALL	20004
#define ID_BTN_CANCEL		20005

#define COMPOENT_DISTANCE	20

#define TEXT_HIGHT			25
#define TEXT_WIDTH			((BUTTON_WIDTH+COMPOENT_DISTANCE)*5-COMPOENT_DISTANCE)

#define BUTTON_HIGHT		25
#define BUTTON_WIDTH		100

// wparam:classname, lparam:text
#define WM_BLOCK_SETSTRINFO	WM_USER+1
// wparam:hwnd, lparam: null
#define WM_BLOCK_SETHWND	WM_USER+2
// wparam:type, lparam: pointer
#define WM_BLOCK_CALLBACK	WM_USER+3
#define BLOCK_CALLBACK_CLASS	1
#define BLOCK_CALLBACK_TEXT		2
#define BLOCK_CALLBACK_HWND		3

// wparam: class, lparam: text
#define WM_BLOCK_BOTH_CALLBACK WM_USER+4



int registerBlkWndClass(HINSTANCE hInstance);
HWND createBlkSetWnd(HINSTANCE hinstance, HWND hparent);

