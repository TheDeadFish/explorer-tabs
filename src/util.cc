#include <windows.h>
#include "util.h"


WCHAR* getWindowText(HWND hwnd)
{
	WCHAR buff[1984];
	GetWindowTextW(hwnd, buff, 1984);
	return wcsdup(buff);
}
