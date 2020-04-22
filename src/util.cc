#include <windows.h>
#include "util.h"


WCHAR* getWindowText(HWND hwnd)
{
	WCHAR buff[1984];
	GetWindowTextW(hwnd, buff, 1984);
	return wcsdup(buff);
}


SIZE getWindowSize(HWND hwnd)
{
	RECT rc; GetWindowRect(hwnd, &rc);
	return {rc.right-rc.left, rc.bottom-rc.top};
}
