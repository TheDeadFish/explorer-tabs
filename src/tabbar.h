#pragma once

extern WNDPROC tabbar_prevProc;
void tabbar_init();
LRESULT CALLBACK tabbar_hookProc(HWND hwnd, 
	UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	
enum {
	MSG_CREATE = WM_APP, 
	MSG_DESTROY, MSG_TEXT,
	MSG_DRAW
};

void tabbar_regClass();
void tabbar_msgRecv(UINT uMsg,
	WPARAM wParam, LPARAM lParam);
		
void tabbar_setProp(HWND hwnd, void* slot);
void* tabbar_getProp(HWND hwnd);
void* tabbar_findTab();
