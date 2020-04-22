#include <windows.h>
#include <win32hlp.h>
#include "tabbar.h"
#include <conio.h>
#include "util.h"

static
void getBarRect(HWND hwnd, RECT* rc)
{
	GetWindowRect(hwnd, rc);
	rc->right -= rc->left+84;
	rc->left = 24; rc->top = 4;
	rc->bottom = 23;
}

struct TabBar
{
	struct Tab {
		Tab *next, *prev;	
		TabBar *tabBar;
		HWND hwnd;
		WCHAR *name, *path;
		
		void remove() { tabBar->remove(this); }
		void move() { tabBar->move(this); }
	};

	Tab *firstTab;
	Tab *curTab;
	
	
	
	void add(HWND hwnd);
	void remove(Tab* tab);
	
	
	void move(Tab* tab);
	
	
};

static TabBar::Tab* tabbar_getTab(HWND hwnd) {
	return (TabBar::Tab*)tabbar_getProp(hwnd); }

void TabBar::add(HWND hwnd)
{
	Tab* tab = (Tab*)calloc(1, sizeof(Tab));
	tab->tabBar = this; tab->hwnd = hwnd;
	insert_before_root(firstTab, tab);
	curTab = tab; tabbar_setProp(hwnd, tab);
}

#if 0

void TabBar::remove(Tab* tab)
{
	remove_before_root(firstTab, tab); free(tab);	
	if(curTab == tab) curTab = NULL;
	if(!firstTab) DestroyWindow(hwnd);
}

LRESULT CALLBACK tabbar_wndProc(HWND hwnd, 
	UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	INIT_WND_CONTEXT(TabBar, This->hwnd = hwnd);
	
	
	WNDMSG_SWITCH(
	  ON_MESSAGE(WM_NCDESTROY, free(This))
		ON_MESSAGE(WM_NCHITTEST, return HTCAPTION)
	,)
	
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static
BOOL CALLBACK EnumWindowsProc(
	HWND hwnd, LPARAM lParam)
{
	if((IsWindowVisible(hwnd))
	&&(s_clssAtom == GetClassLong(hwnd, GCW_ATOM)))
		*(HWND*)lParam = hwnd; return FALSE;
	return TRUE;
}

TabBar* findTabBar(void)
{
	HWND hwnd = NULL;
	EnumWindows(EnumWindowsProc, (LPARAM)&hwnd);
	if(hwnd) return (TabBar*)
		GetWindowLongPtr(hwnd, GWLP_USERDATA);
	return NULL;
}

#endif


TabBar* createTabBar(void)
{
	TabBar* This = (TabBar*) calloc(1, sizeof(TabBar));
	
	
	
	return This;
}



TabBar* findTabBar(void)
{
	return NULL;
}

void tabbar_create(HWND hwnd)
{
	TabBar* root = findTabBar();
	if(!root) root = createTabBar();
	root->add(hwnd);
	
}


void tabbar_draw(HWND hwnd, TabBar* tab)
{
	HDC hdc = GetWindowDC(hwnd);
	RECT rc;
	getBarRect(hwnd, &rc);
	FillRect(hdc, &rc, 
		GetSysColorBrush(COLOR_3DFACE));
	ReleaseDC(hwnd, hdc);
}

void tabbar_msgRecv(UINT uMsg,
	WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)wParam;
	if(uMsg == MSG_CREATE) { 
		tabbar_create(hwnd); return; }
		
	if((uMsg == MSG_TEXT)
	||(uMsg == MSG_DRAW))
		tabbar_draw(hwnd, 0);
		
		
	//auto* tab = tabbar_getTab(hwnd);
	
	//if(
	
	
	
	/*if(!tab) {
		if(uMsg == MSG_TEXT) {
	
	
	}
	
	if(uMsg == MSG_TEXT) {
		if(!tab)
	
		free((void*)lParam);
	
	
	}
	
	
	
	
	//_cprintf("!!! %X\n", tab);
	*/

	
	

	//if(tab) { if(uMsg == MSG_DESTROY) tab->remove(); 
	//	if(uMsg == MSG_MOVE) tab->move(); }
}

void tabbar_regClass(void)
{
	//WNDCLASSEXW wc = {sizeof(WNDCLASSEX)};
//	wc.lpfnWndProc   = tabbar_wndProc;
	//wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	//wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	//wc.lpszClassName = L"dfTabBar";

	//s_clssAtom = RegisterClassExW(&wc);
}
