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
		TabBar *tabBar;
		HWND hwnd;
		WCHAR *name, *path;
		
		void remove() { tabBar->remove(this); }
		void move() { tabBar->move(this); }
		bool isSel() { return tabBar->isSel(this); }
	};


	Tab** tabs; int nTabs;
	int nTabScroll, nTabSel;
	int nTabWidth, nWndWidth;
	
	
	
	
	int scroll;
	
	RECT rc;
	
	
	
	
	
	
	void add(HWND hwnd);
	void remove(Tab* tab);
	
	
	void move(Tab* tab);
	
	void draw(HWND hwnd);
	
	void size(HWND hwnd);
	
	
	RECT getRect() { return rc; }
	int clipRight() { return rc.right-scroll; }
	
	void add(Tab* tab);
	
	
	bool isSel(Tab* tab) { return 
		tabs[nTabSel] == tab; }
		
		
		
	RECT getRect(int i);
		
	
};

static TabBar::Tab* tabbar_getTab(HWND hwnd) {
	return (TabBar::Tab*)tabbar_getProp(hwnd); }
	
RECT TabBar::getRect(int i)
{
	int x1 = rc.left+scroll; 
	int x2 = rc.right-scroll;
	if(i < 0) { swapReg(x1, x2);
		if(i == -1) x1 = rc.left;
		else x2 = rc.right; 
	} else { x1 += (i-nTabScroll)*nTabWidth;
		if((i+1) != nTabs) min_ref(x2, x1+nTabWidth); }
	return {x1, rc.top, x2, rc.bottom};
}
	
void TabBar::add(Tab* tab)
{
	nTabSel = nTabs;
	tabs = (Tab**)realloc(tabs,
		(++nTabs)*sizeof(Tab*));
	tabs[nTabSel] = tab;
	nWndWidth = INT_MIN;
}

void TabBar::add(HWND hwnd)
{
	Tab* tab = (Tab*)calloc(1, sizeof(Tab));
	tab->tabBar = this; tab->hwnd = hwnd;
	tabbar_setProp(hwnd, tab);
	this->add(tab);
}

void TabBar::size(HWND hwnd)
{
	int width = getWindowSize(hwnd).cx;
	if(nWndWidth == width) return;
	nWndWidth = width;
	rc = {23, 4, nWndWidth-58, 22};
	width = rc.right-rc.left;
		
	// calculate tab width
	nTabWidth = width / nTabs;
	if(nTabWidth >= MIN_TAB_WIDTH) {
		scroll = 0; nTabScroll = 0; return; }
	scroll = TAB_BTN_WIDTH;
		
	// adjust nTabIndex
	int nDispTab = (width-TAB_BTN_WIDTH*2) / nTabWidth;
	max_ref(nTabScroll, nTabSel);
	max_ref(nTabScroll, (nTabSel-nDispTab)+1);
	min_ref(nTabScroll, nTabs-nDispTab);
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
	TabBar* This = (TabBar*) 
		calloc(1, sizeof(TabBar));
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
	if(nTabs < 2) return;
	size(hwnd);

	// erase background
	HDC hdc = GetWindowDC(hwnd);
	BOOL active = GetForegroundWindow() == hwnd;
	FillCaptionGradient(hdc, &rc, active);
	
	
	// setup text
	SelectObject(hdc, s_hCaptionFont);
	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, GetStockObject(DC_PEN));
	SetDCPenColor(hdc, GetSysColor(
		COLOR_INACTIVECAPTIONTEXT));
	
	for(int i = nTabScroll; i < nTabs; i++)
	{	
		RECT rc = getRect(i);
			
		// draw the divider
		if(i != nTabScroll) {
			MoveToEx(hdc, rc.left, this->rc.top, NULL);
			LineTo(hdc, rc.left, this->rc.bottom);
		 rc.left += 4; }

		// draw the caption text
		rc.right -= 2; rc.top += 2; rc.left += 1;
		setCaptionColor(hdc, active && (i == nTabSel));
		DrawTextW(hdc, tabs[i]->name, -1, &rc, 0);
		
		if(rc.right+2 > clipRight()) {
			goto RIGHT_ENABLE;
		}
	}
	
	// draw buttons
	if(scroll) { int flags;
	
		// draw right button
		flags = DFCS_SCROLLRIGHT|DFCS_INACTIVE;
		if(0) { RIGHT_ENABLE: flags = DFCS_SCROLLRIGHT; }
		int tmp = rc.left; rc.left = clipRight();
		DrawFrameControl(hdc, &rc, DFC_SCROLL, flags);
		rc.left = tmp;
		
		// draw left button 
		flags = DFCS_SCROLLLEFT|DFCS_INACTIVE;
		if(nTabScroll) flags = DFCS_SCROLLLEFT;
		tmp = rc.right; rc.right = rc.left + scroll;
		DrawFrameControl(hdc, &rc, DFC_SCROLL, flags);
		rc.right = tmp;
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
