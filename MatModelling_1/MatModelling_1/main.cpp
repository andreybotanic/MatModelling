#include <windows.h>
#include <windowsx.h>
#include "window.h"
#include <string>
#include <gdiplus.h>

using namespace Gdiplus;

HPEN axisPen, f1Pen, f2Pen;
HBRUSH axisBr;
POINT axisArrow[3];
HFONT font;
RECT txtPos;
char text[5];
int num_step;
int min_x = 10, min_y = 5, cnt_x, cnt_y;
int xO, yO;
float scale = 20;
int pointsPerStep = 10;

float F1(float x) {
	return sin(x);
}

float F2(float x) {
	return cos(x);
}

void drawFunc(Window wnd, WNDINFO wndinfo, float (*func)(float), HPEN pen) {
	wnd.SetPen(pen);
	float x = 0, y = func(0);
	float dx = pointsPerStep / scale / num_step;
	MoveToEx(wndinfo.buffer_dc, xO + x * scale, yO - y * scale, NULL);
	do {
		x += dx;
		y = func(x);
		LineTo(wndinfo.buffer_dc, xO + x * scale, yO - y * scale);
	} while (xO + x * scale <= wndinfo.width);
	x = 0;
	y = func(0);
	MoveToEx(wndinfo.buffer_dc, xO + x * scale, yO - y * scale, NULL);
	do {
		x -= dx;
		y = func(x);
		LineTo(wndinfo.buffer_dc, xO + x * scale, yO - y * scale);
	} while (xO + x * scale >= 0);
}

void drawAxis(Window wnd, WNDINFO wndinfo) {
	wnd.SetPen(axisPen);
	wnd.SetBrush(axisBr);
	xO = wndinfo.width / 2;
	yO = wndinfo.height / 2;

	MoveToEx(wndinfo.buffer_dc, 10, yO, NULL);
	LineTo(wndinfo.buffer_dc, wndinfo.width - 10, yO);
	MoveToEx(wndinfo.buffer_dc, xO, 10, NULL);
	LineTo(wndinfo.buffer_dc, xO, wndinfo.height - 10);

	axisArrow[0] = { xO, 10 };
	axisArrow[1] = { xO + 3, 20 };
	axisArrow[2] = { xO - 3, 20 };
	Polygon(wndinfo.buffer_dc, axisArrow, 3);
	axisArrow[0] = { wndinfo.width - 10, yO };
	axisArrow[1] = { wndinfo.width - 20, yO + 3 };
	axisArrow[2] = { wndinfo.width - 20, yO  - 3};
	Polygon(wndinfo.buffer_dc, axisArrow, 3);

	float px_step = 20;
	num_step = 1;
	if (scale < px_step) {
		num_step = ceil(px_step / scale);
		px_step = num_step * scale;
	}
	else {
		px_step = scale;
	}
	if (!px_step) {
		px_step = 1;
	}
	cnt_x = (wndinfo.width - 42) / 2 / px_step, cnt_y = (wndinfo.height - 42) / 2 / px_step;
	if (min_x % num_step == 0 && cnt_x * num_step < min_x) {
		cnt_x++;
	}
	if (min_y % num_step == 0 && cnt_y * num_step < min_y) {
		cnt_y++;
	}
	SelectObject(wndinfo.buffer_dc, font);
	txtPos.top = yO + 5;
	txtPos.bottom = yO + 15;
	for (int i = 1; i <= cnt_x; i++) {
		MoveToEx(wndinfo.buffer_dc, xO + px_step * i, yO - 3, NULL);
		LineTo(wndinfo.buffer_dc, xO + px_step * i, yO + 4);
		MoveToEx(wndinfo.buffer_dc, xO - px_step * i, yO - 3, NULL);
		LineTo(wndinfo.buffer_dc, xO - px_step * i, yO + 4);
		txtPos.left = xO + px_step * i - 10;
		txtPos.right = xO + px_step * i + 10;
		_itoa_s(i * num_step, text, 5, 10);
		DrawText(wndinfo.buffer_dc, text, strlen(text), &txtPos, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		txtPos.left = xO - px_step * i - 10;
		txtPos.right = xO - px_step * i + 10;
		_itoa_s((-i) * num_step, text, 5, 10);
		DrawText(wndinfo.buffer_dc, text, strlen(text), &txtPos, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}
	txtPos.left = xO + 5;
	txtPos.right = xO + 15;
	for (int i = 1; i <= cnt_y; i++) {
		MoveToEx(wndinfo.buffer_dc, xO - 3, yO + px_step * i, NULL);
		LineTo(wndinfo.buffer_dc, xO + 4, yO + px_step * i);
		MoveToEx(wndinfo.buffer_dc, xO - 3, yO - px_step * i, NULL);
		LineTo(wndinfo.buffer_dc, xO + 4, yO - px_step * i);
		txtPos.top = yO + px_step * i + 5;
		txtPos.bottom = yO +px_step * i - 5;
		_itoa_s((-i) * num_step, text, 5, 10);
		DrawText(wndinfo.buffer_dc, text, strlen(text), &txtPos, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		txtPos.top = yO - px_step * i + 5;
		txtPos.bottom = yO - px_step * i - 5;
		_itoa_s(i * num_step, text, 5, 10);
		DrawText(wndinfo.buffer_dc, text, strlen(text), &txtPos, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}
}

void resize(Window wnd, WNDINFO wndinfo) {
	scale = min((float)(wndinfo.width - 42) / 2 / min_x, (float)(wndinfo.height - 42) / 2 / min_y);
	wnd.Clear();
	drawAxis(wnd, wndinfo);
	drawFunc(wnd, wndinfo, F1, f1Pen);
	drawFunc(wnd, wndinfo, F2, f2Pen);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
	MSG Msg;
	Window myWindow(hInstance, nCmdShow, 300, 300);

	axisPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	f1Pen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	f2Pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	axisBr = CreateSolidBrush(RGB(0, 0, 0));
	font = CreateFont(12, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Arial");
	myWindow.SetBackground(RGB(255, 255, 255));
	myWindow.OnResizeFunction(resize);

	while (GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}