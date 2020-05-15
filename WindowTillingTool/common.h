#pragma once
#include <Windows.h>



#define APP_TITLE_MAIN "WinTool"
#define APP_WIN_CLASS_MAIN "WinTool_Window_Class_Main"
#define APP_TITLE_CONSOLE "WinTool_Main_Console_FANGZHICHONGMING"

#define APP_WIN_CLASS_BLOCK "WinTool_Window_Class_Block"

#define BUFFER_SIZE 256


#define INI_FILE_NAME	"wtiletool.ini"

// There strings are for the configuation
#define SECNAME_TILE_SETTING "Tile Setting"
#define SC_TILE_PADDINGX "paddingx"
#define SC_TILE_PADDINGY "paddingy"

#define SECNAME_CLASS_BLOCK_LIST "Class Block List"

#define SECNAME_TEXT_BLOCK_LIST "Text Block List"

#define SECNAME_CLASS_PART_BLOCK_LIST "Class Part Block List"

#define SECNAME_TEXT_PART_BLOCK_LIST "Text Block List"

#define SECNAME_BOTH_BLOCK_LIST "Both Block List"

void ReadjustWindow(HWND hWnd, int nWidth, int nHeight);

