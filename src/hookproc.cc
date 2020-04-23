#include <windows.h>
#include "tabbar.h"
#include "util.h"

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

void tabbar_setProp(HWND hwnd, tabbar_t* slot) 
{
	SetPropW(hwnd, MAKEINTATOM(
		s_propAtom), (HANDLE)slot); 
}

tabbar_t* tabbar_getProp(HWND hwnd) 
{
	return (tabbar_t*)GetPropW(hwnd,
		MAKEINTATOM(s_propAtom)); 
}

void tabbar_init(void)
{
	CreateThread(0, 0, ThreadProc, 0, 0, &s_idThread);
	s_propAtom = GlobalAddAtomA("dfTabBar");
}


#include <conio.h>

void tabbar_msgSend(tabbar_t* tab, int msg, void* arg=0)
{
	if(tab) PostThreadMessage(s_idThread, 
		msg, (WPARAM)tab, (LPARAM)arg);
}

void tabbar_msgSend(HWND hwnd, int msg, void* arg=0)
{
	tabbar_msgSend(tabbar_getProp(hwnd), msg, arg);
}


static
LRESULT tabbar_mouse(HWND hwnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam)
{
	// get tab object
	tabbar_t* This = tabbar_getProp(hwnd);
	if(!This) return 0;
	
	// mouse button down
	if(uMsg == WM_NCLBUTTONDOWN) {
		if((wParam != HTCAPTION)
		||(GetForegroundWindow() != hwnd)) {
			This->lbPend = 0; return 0; }
		This->lbPend = lParam; return 1;
	}
	
	// mouse move
	if(This->lbPend == 0) return 0;
	if(uMsg == WM_NCMOUSEMOVE) {
		tabbar_prevProc(hwnd, WM_NCLBUTTONDOWN,
			HTCAPTION, This->lbPend);
		This->lbPend = 0; return 0;
	}
	
	// mouse button up
	tabbar_msgSend(This, MSG_MOUSE, (void*)
		screnToWindow(hwnd, This->lbPend));
	This->lbPend = 0; return 0;	
}

void tabbar_create(HWND hwnd)
{
	tabbar_t* This = tabbar_allocTab(hwnd);
	tabbar_setProp(hwnd, This);
	tabbar_msgSend(This, MSG_CREATE);
}

LRESULT CALLBACK tabbar_hookProc(HWND hwnd, 
	UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	if((uMsg == WM_NCMOUSEMOVE)
	||(uMsg == WM_NCLBUTTONDOWN)
	||(uMsg == WM_NCLBUTTONUP)) {
		IFRET(tabbar_mouse(hwnd, 
			uMsg, wParam, lParam));
	}

	LRESULT lResult = tabbar_prevProc(
		hwnd, uMsg, wParam, lParam);
		
	switch(uMsg) {
	case WM_CREATE:
		tabbar_create(hwnd); break;
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
	DWORD pid; tabbar_t* tab;
	if(IsWindowVisible(hwnd)) { 
		GetWindowThreadProcessId(hwnd, &pid);
		if((pid == GetCurrentProcessId())
		&&(tab = tabbar_getProp(hwnd))&&(tab->tabBar)) {
			*(void**)lParam = tab->tabBar; return FALSE; }
	} return TRUE;
}

TabBar* tabbar_findTabBar()
{
	TabBar* tab = NULL;
	EnumWindows(EnumWindowsProc, (LPARAM)&tab);
	return tab;	
}
