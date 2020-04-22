#include <windows.h>
#include <win32hlp.h>
#include "tabbar.h"
#include <conio.h>
#include "util.h"

#define MIN_TAB_WIDTH 100
#define TAB_BTN_WIDTH 16

HFONT s_hCaptionFont;
int s_gcyCaptionFontChar;

struct TabBar
{
	struct Tab {
		Tab *next;	
		TabBar *tabBar;
		HWND hwnd;
		WCHAR *name, *path;
		
		void remove() { tabBar->remove(this); }
		void move() { tabBar->move(this); }
		
		bool isSel() { return tabBar->curTab == this; }
	};




	Tab *firstTab;
	Tab *curTab;
	
	int nTabIndex;
	int nTabWidth;
	
	int nWndWidth;
	
	
	
	
	bool scroll;
	
	RECT rc;
	
	
	
	
	
	
	void add(HWND hwnd);
	void remove(Tab* tab);
	
	
	void move(Tab* tab);
	
	void draw(HWND hwnd);
	
	void size(HWND hwnd);
	
	
	RECT getRect() { return rc; }
	
	
	
	
	
	
};

static TabBar::Tab* tabbar_getTab(HWND hwnd) {
	return (TabBar::Tab*)tabbar_getProp(hwnd); }

void TabBar::add(HWND hwnd)
{
	Tab* tab = (Tab*)calloc(1, sizeof(Tab));
	tab->tabBar = this; tab->hwnd = hwnd;
	fwdlst_insert_end(&firstTab, tab);
	curTab = tab; tabbar_setProp(hwnd, tab);
	nWndWidth = INT_MIN;
}

void TabBar::size(HWND hwnd)
{
	int width = getWindowSize(hwnd).cx;
	if(nWndWidth == width) return;
	nWndWidth = width;
	rc = {24, 4, nWndWidth-60, 22};
	
	// count number of tabs
	int nTabs = 0; int nCurTab = 0;
	for(Tab* tab = firstTab; tab; tab = tab->next) {
		if(tab == curTab) nCurTab = nTabs; nTabs++; }
	_cprintf("xxx %d\n", nTabs);
		
	// calculate tab width
	nTabWidth = width / nTabs;
	if(nTabWidth >= MIN_TAB_WIDTH) {
		scroll = false; nTabIndex = 0; return; }
		
	// adjust nTabIndex
	int nDispTab = (width-TAB_BTN_WIDTH*2) / nTabWidth;
	int minIndex = (nCurTab-nDispTab)+1;
	int maxIndex = (nTabs-nDispTab);
	if(nTabIndex > nCurTab) nTabIndex = nCurTab;
	if(nTabIndex < minIndex) nTabIndex = minIndex;
	if(nTabIndex > maxIndex) nTabIndex = maxIndex;
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


static
TabBar::Tab* findTab(void) { return 
	(TabBar::Tab*)tabbar_findTab(); }

TabBar* findTabBar(void)
{
	auto tab = findTab();
	if(tab) return tab->tabBar;
	return NULL;
}

void tabbar_create(HWND hwnd)
{
	TabBar* root = findTabBar();
	if(!root) root = createTabBar();
	root->add(hwnd);
	
}




void TabBar::draw(HWND hwnd)
{
	if(!firstTab) return;
	size(hwnd);

	// erase background
	HDC hdc = GetWindowDC(hwnd);
	RECT rc = getRect();
	BOOL active = GetForegroundWindow() == hwnd;
	FillCaptionGradient(hdc, &rc, active);
	
	
	// setup text
	SelectObject(hdc, s_hCaptionFont);
	SetBkMode(hdc, TRANSPARENT);
	rc.top += 2; 

	int nTabs = 0;
	for(Tab* tab = firstTab; tab; tab = tab->next) {
		if(nTabs++ < nTabIndex) continue;
		
		setCaptionColor(hdc, active && (tab == curTab));
		rc.right = rc.left + nTabWidth;
		DrawTextW(hdc, tab->name, -1, &rc, 0);
		
		
		
		
		rc.left += nTabWidth;
		
	}
		
	ReleaseDC(hwnd, hdc);
}

void tabbar_msgRecv(UINT uMsg,
	WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)wParam;
	if(uMsg == MSG_CREATE) { 
		tabbar_create(hwnd); return; }
		
	auto* tab = tabbar_getTab(hwnd);
	if(!tab) {
		if(uMsg == MSG_TEXT) free((void*)lParam);
		return ; }
		
		
	if(uMsg == MSG_TEXT) {
		tab->name = (WCHAR*)lParam; 
		tab->tabBar->draw(hwnd); }
		
	if(uMsg == MSG_DRAW) {
		if(tab->isSel()) {
			tab->tabBar->draw(hwnd);  }
	}
}

void tabbar_regClass(void)
{
	s_hCaptionFont = getCaptionFont();
	SIZE size = getFontDimention(s_hCaptionFont);
	s_gcyCaptionFontChar = size.cy;
}
