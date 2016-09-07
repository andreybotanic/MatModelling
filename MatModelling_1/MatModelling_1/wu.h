#pragma once
#include <windows.h>
#include <windowsx.h>

COLORREF interpolateColor(COLORREF start, COLORREF end, float part) {
	BYTE r, g, b;
	r = (GetRValue(end) - GetRValue(start)) * part + GetRValue(start);
	g = (GetGValue(end) - GetGValue(start)) * part + GetGValue(start);
	b = (GetBValue(end) - GetBValue(start)) * part + GetBValue(start);
	return RGB(r, g, b);
}

void BresenhamLine(HDC dc, int x0, int y0, int x1, int y1, int width, COLORREF color) {
	int deltax = abs(x1 - x0), deltay = abs(y1 - y0);
	int error = 0;
	int deltaerr = deltay;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		SetPixel(dc, x, y, color);
		error = error + deltaerr;
		if (2 * error >= deltax) {
			y = y - 1;
			error = error - deltax;
		}
	}
}

void WuLine(HDC dc, int x0, int y0, int x1, int y1, int width, COLORREF color, COLORREF bgc = RGB(255, 255, 255)) {
	for (int x = x0; x <= x1; x++) {
		for (int y = y0; y <= y0 + width / 2; y0++) {

		}
	}
}