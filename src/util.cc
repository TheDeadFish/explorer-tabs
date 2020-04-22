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


void FillGradient(HDC hdc, LPCRECT prc, COLORREF rgbLeft, COLORREF rgbRight)
{
	TRIVERTEX avert[4];
	static GRADIENT_RECT auRect[1] = {0,1};
	#define GetCOLOR16(RGB, clr) ((COLOR16)(Get ## RGB ## Value(clr) << 8))

	avert[0].Red = GetCOLOR16(R, rgbLeft);
	avert[0].Green = GetCOLOR16(G, rgbLeft);
	avert[0].Blue = GetCOLOR16(B, rgbLeft);

	avert[1].Red = GetCOLOR16(R, rgbRight);
	avert[1].Green = GetCOLOR16(G, rgbRight);
	avert[1].Blue = GetCOLOR16(B, rgbRight);

	avert[0].x = prc->left;
	avert[0].y = prc->top;
	avert[1].x = prc->right;
	avert[1].y = prc->bottom;

	GdiGradientFill(hdc, avert, 2,(PVOID)auRect, 1, GRADIENT_FILL_RECT_H);
}

void FillCaptionGradient(HDC hdc, LPCRECT prc, BOOL fActive)
{
	COLORREF rgbLeft, rgbRight;

	if (fActive) {
		rgbLeft = GetSysColor(COLOR_ACTIVECAPTION);
		rgbRight = GetSysColor(COLOR_GRADIENTACTIVECAPTION);
	} else {
		rgbLeft = GetSysColor(COLOR_INACTIVECAPTION);
		rgbRight = GetSysColor(COLOR_GRADIENTINACTIVECAPTION);
	}
	//if (rgbLeft != rgbRight) {
		FillGradient(hdc, prc, rgbLeft, rgbRight);
	//} else {
	//	FillRect(hdc, prc, fActive ? SYSHBR(ACTIVECAPTION) : SYSHBR(INACTIVECAPTION));
	//}
}

HFONT getCaptionFont()
{
	NONCLIENTMETRICS ncm; ncm.cbSize = sizeof(ncm);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
	return CreateFontIndirect(&ncm.lfCaptionFont);
}

SIZE getFontDimention(HFONT hFont)
{
	HDC hdc = GetDC(NULL);
	SelectObject(hdc, hFont); LONG height;
	LONG width = GdiGetCharDimensions(hdc, 0, &height);
	ReleaseDC(NULL, hdc);
	return {width, height};
}

void setCaptionColor(HDC hdc, BOOL fActive)
{
	COLORREF color;
	if(fActive) { color = GetSysColor(COLOR_CAPTIONTEXT);
	} else { color = GetSysColor(COLOR_INACTIVECAPTIONTEXT); }
	SetTextColor(hdc, color);
}
