#include <windows.h>
#include "tabbar.h"

WNDPROC tabbar_prevProc;
static DWORD s_idThread;
static ATOM s_propAtom;


static 
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	tabbar_regClass();
	
	// main message loop
	MSG Msg;
	while(GetMessage(&Msg, NULL, 0, 0) > 0)	{
		if(Msg.hwnd == NULL)
			tabbar_msgRecv(Msg.message, Msg.wParam, Msg.lParam);
		else
			TranslateMessage(&Msg);	DispatchMessage(&Msg); 
	}
	return 0;
}

void tabbar_setProp(HWND hwnd, void* slot) 
{
	SetPropW(hwnd, MAKEINTATOM(
		s_propAtom), (HANDLE)slot); 
}

void* tabbar_getProp(HWND hwnd) 
{
	return (void*)GetPropW(hwnd,
		MAKEINTATOM(s_propAtom)); 
}

void tabbar_init(void)
{
	CreateThread(0, 0, ThreadProc, 0, 0, &s_idThread);
	s_propAtom = GlobalAddAtomA("dfTabBar");
}

void tabbar_msgSend(HWND hwnd, int msg, void* arg=0)
{
	PostThreadMessage(s_idThread, msg, (WPARAM)hwnd, (LPARAM)arg);
}


LRESULT CALLBACK tabbar_hookProc(HWND hwnd, 
	UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = tabbar_prevProc(
		hwnd, uMsg, wParam, lParam);
		
	switch(uMsg) {
	case WM_CREATE:
		tabbar_msgSend(hwnd, MSG_CREATE); break;
	case WM_NCDESTROY:
		tabbar_msgSend(hwnd, MSG_DESTROY); break;
	case WM_SETTEXT:
		tabbar_msgSend(hwnd, MSG_TEXT, 
			wcsdup((WCHAR*)lParam)); break;
	case WM_NCPAINT:
	case WM_NCACTIVATE:
		tabbar_msgSend(hwnd, MSG_DRAW); break;
	}
	
	return lResult;
}

static
BOOL CALLBACK EnumWindowsProc(
	HWND hwnd, LPARAM lParam)
{
	DWORD pid; void* tab;
	if(IsWindowVisible(hwnd)) { 
		GetWindowThreadProcessId(hwnd, &pid);
		if((pid == GetCurrentProcessId())
		&&(tab = tabbar_getProp(hwnd))) {
			*(void**)lParam = tab; return FALSE; }
	}
	return TRUE;
}

void* tabbar_findTab()
{
	void* tab = NULL;
	EnumWindows(EnumWindowsProc, (LPARAM)&tab);
	return tab;	
}
