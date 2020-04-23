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
	struct Tab : tabbar_t {
		TabBar *tabBar;
		
		WCHAR *name, *path;
		
		void remove() { tabBar->remove(this); }
		void move() { tabBar->move(this); }
		bool isSel() { return tabBar->isSel(this); }
		void mouse(LPARAM lParam) { tabBar->mouse(lParam); }
		
		
		void draw() { return tabBar->draw(); }
		
		
		
	};
	
	HWND selWnd() { return tabs[nTabSel]->hwnd; }
	
	
	


	Tab** tabs; int nTabs;
	int nTabScroll, nTabSel;
	int nTabWidth, nWndWidth;
	
	
	
	
	int scroll;
	
	RECT rc;
	
	
	
	
	
	
	void add(HWND hwnd);
	void remove(Tab* tab);
	
	
	void move(Tab* tab);
	
	void draw(bool user=false);
	void size(bool user=false);
	
	
	RECT getRect() { return rc; }
	
	
	
	
	int clipRight() { return rc.right-scroll; }
	int clipLeft() { return rc.left+scroll; }
	
	
	void add(Tab* tab);
	
	
	bool isSel(Tab* tab) { return 
		tabs[nTabSel] == tab; }
		
		
		
	bool getRect(RECT& x, int i);
	
	void mouse(LPARAM lParam);
	int hitTest(int xPos);
	
	
	
};

static TabBar::Tab* tabbar_getTab(HWND hwnd) {
	return (TabBar::Tab*)tabbar_getProp(hwnd); }
	
bool TabBar::getRect(RECT& x, int i)
{
	x = {rc.left+scroll, rc.top, 
		rc.right-scroll, rc.bottom};
	x.left += (i-nTabScroll)*nTabWidth;
	
	int x2 = x.left+nTabWidth;
	if(x.right < x2) return true;
	if((i+1) != nTabs) x.right = x2;
	return false;
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

void TabBar::size(bool user)
{
	HWND hwnd = selWnd();
	int width = getWindowSize(hwnd).cx;
	if((nWndWidth == width) && !user) 
		return; nWndWidth = width;
	rc = {23, 4, nWndWidth-58, 22};
	width = rc.right-rc.left;
		
	// calculate tab width
	nTabWidth = width / nTabs;
	if(nTabWidth >= MIN_TAB_WIDTH) {
		scroll = 0; nTabScroll = 0; return; }
	scroll = TAB_BTN_WIDTH;
		
	// adjust nTabIndex
	int nDispTab = (width-TAB_BTN_WIDTH*2) / nTabWidth;
	min_max_ref(nTabScroll, 0, nTabs-nDispTab);
	if(!user) min_max_ref(nTabScroll, 
		(nTabSel-nDispTab)+1, nTabSel);
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




void TabBar::draw(bool user)
{
	if(!nTabs) return; size(user);
	if(nTabs < 2) return;

	// erase background
	HWND hwnd = selWnd();
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
		RECT rc; bool clip;
		clip = getRect(rc, i);
			
		// draw the divider
		if(i != nTabScroll) {
			MoveToEx(hdc, rc.left, this->rc.top, NULL);
			LineTo(hdc, rc.left, this->rc.bottom);
		 rc.left += 4; }

		// draw the caption text
		rc.right -= 2; rc.top += 2; rc.left += 1;
		setCaptionColor(hdc, active && (i == nTabSel));
		DrawTextW(hdc, tabs[i]->name, -1, &rc, 0);
		
		if(clip) goto RIGHT_ENABLE;
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


int TabBar::hitTest(int xPos)
{
	if(xInRect(rc, xPos)) {
		if(xPos < clipLeft()) return -1;
		if(xPos >= clipRight()) return -2;
		for(int i = nTabScroll; i < nTabs; i++) {
			RECT rc; getRect(rc, i);
			if(xInRect(rc, xPos)) return i; }
	} return INT_MIN;
}

void TabBar::mouse(LPARAM lParam)
{
	int hit = hitTest(GET_X_LPARAM(lParam));
	if(hit < 0) {
		if(hit == -1) { nTabScroll--; draw(true); }
		if(hit == -2) { nTabScroll++; draw(true); }
	
	
	
	
	
	}
	
	
	
	
	
	//_cprintf("hit: %d\n", hit);
	
	
	


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
		
	if(uMsg == MSG_MOUSE) {
		tab->mouse(lParam);
	}
		
		
	if(uMsg == MSG_TEXT) {
		tab->name = (WCHAR*)lParam; 
		tab->draw(); }
		
	if(uMsg == MSG_DRAW) {
	
	
	
	
	
	
	
		if(tab->isSel()) {
			tab->draw();  }
	}
}

void tabbar_regClass(void)
{
	s_hCaptionFont = getCaptionFont();
	SIZE size = getFontDimention(s_hCaptionFont);
	s_gcyCaptionFontChar = size.cy;
}
