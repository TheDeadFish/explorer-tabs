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
		
		
		WCHAR *name, *path;
		
		void setName(WCHAR* name_) {
			free(name); name = name_; }
		
		void remove() { tabBar->remove(this); }
		void move(WINDOWPLACEMENT* wp) { tabBar->move(this, wp); }
		bool isSel() { return tabBar->isSel(this); }
		void mouse(LPARAM lParam) { tabBar->mouse(lParam); }
		
		
		void draw() { return tabBar->draw(); }
		
		
		
	};
	
	HWND selWnd() { return tabs[nTabSel]->hwnd; }
	
	
	


	Tab** tabs; int nTabs;
	int nTabScroll, nTabSel;
	int nTabWidth, nWndWidth;
	
	WINDOWPLACEMENT wndPlace;
	
	
	
	
	
	int scroll;
	
	RECT rc;
	
	
	
	
	
	
	void add(HWND hwnd);
	void remove(Tab* tab);
	
	
	void move(Tab* tab, WINDOWPLACEMENT* wp);
	
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
	
	void switch_window(int iTab);
	
	
	int tabIndex(Tab* tab);
};
	
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
	tab->tabBar = this; 
	int iTab = nTabs;
	tabs = (Tab**)realloc(tabs,
		(++nTabs)*sizeof(Tab*));
	tabs[iTab] = tab;
	nWndWidth = INT_MIN;
	switch_window(iTab);
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

int TabBar::tabIndex(Tab* tab)
{
	for(int i = 0; i < nTabs; i++) {
		if(tabs[i] == tab) return i; }
	__builtin_trap();
}

void TabBar::remove(Tab* tab)
{
	// switch to next tab
	int iTab = tabIndex(tab);
	//_cprintf("remove: %d\n", iTab);
	
	
	if(iTab == nTabSel) {
		switch_window((iTab+1 < nTabs) 
			? iTab+1 : iTab-1); 
	}
	
	// destroy tab
	memcpy(tabs+iTab, tabs+iTab+1, 
		sizeof(Tab)*(--nTabs-iTab));
	free(tab);
}

TabBar* createTabBar(void)
{
	TabBar* This = (TabBar*) 
		calloc(1, sizeof(TabBar));
	return This;
}

void tabbar_add(TabBar::Tab* tab)
{
	TabBar* root = tabbar_findTabBar();
	if(!root) root = createTabBar();
	root->add(tab);
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
	} else { switch_window(hit); }
}

void TabBar::switch_window(int iTab)
{
	//_cprintf("switch_window: %d\n", iTab);

	if(unsigned(iTab) >= nTabs) return;
	int nTabSelPrev = nTabSel; nTabSel = iTab;
	if(nTabSelPrev < 0) return;
	HWND hNextWnd = tabs[nTabSel]->hwnd;
	HWND hPrevWnd = tabs[nTabSelPrev]->hwnd;
	
	if(hNextWnd != hPrevWnd) {
		ShowWindowAsync(hPrevWnd, SW_MINIMIZE);	
		SetWindowPlacement(hNextWnd, &wndPlace); 
	}
	
	SetForegroundWindow(hNextWnd);
}

void TabBar::move(Tab* tab, WINDOWPLACEMENT* wp)
{
	//_cprintf("move: %X, %X, %d\n", tab, wp->showCmd, wp->rcNormalPosition.left);
	if((wp->showCmd != SW_MINIMIZE)
	&&(wp->rcNormalPosition.left < 10000)
	&&(nTabSel == tabIndex(tab)))
		wndPlace = *wp; 
	free(wp);
}

void tabbar_msgRecv(UINT uMsg,
	WPARAM wParam, LPARAM lParam)
{
	auto* tab = (TabBar::Tab*)wParam;

	switch(uMsg) {
	case MSG_CREATE: tabbar_add(tab); break;
	case MSG_DESTROY: tab->remove(); break;
	case MSG_MOUSE: tab->mouse(lParam); break;
	case MSG_TEXT: tab->setName((WCHAR*)lParam);
	case MSG_DRAW: tab->draw(); break;
	case MSG_MOVE: tab->move((WINDOWPLACEMENT*)lParam);
		break;
	}
}

tabbar_t* tabbar_allocTab(HWND hwnd)
{
	tabbar_t* tab = (tabbar_t*)
		calloc(1, sizeof(TabBar::Tab));
	tab->hwnd = hwnd;
	return tab;
}

void tabbar_regClass(void)
{
	s_hCaptionFont = getCaptionFont();
	SIZE size = getFontDimention(s_hCaptionFont);
	s_gcyCaptionFontChar = size.cy;
}
