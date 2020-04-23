#pragma once

extern WNDPROC tabbar_prevProc;
void tabbar_init();
LRESULT CALLBACK tabbar_hookProc(HWND hwnd, 
	UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	
enum {
	MSG_CREATE = WM_APP, 
	MSG_DESTROY, MSG_TEXT,
	MSG_DRAW, MSG_MOUSE, 
	MSG_MOVE
};

void tabbar_regClass();
void tabbar_msgRecv(UINT uMsg,
	WPARAM wParam, LPARAM lParam);

	
struct TabBar;	
struct tabbar_t
{
	HWND hwnd;
	LPARAM lbPend;
	TabBar *tabBar;
};

TabBar* tabbar_findTabBar();
tabbar_t* tabbar_allocTab(HWND hwnd);
