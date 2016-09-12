#include "window.h"
#include <string>

Pen *axisPen, *f1Pen, *f2Pen, *f3Pen, *f4Pen;
SolidBrush *axisBr;
PointF axisArrow[3];
Font *font;
StringFormat *sf;
RectF txtPos;
WCHAR text[5];
int num_step;
int min_x = 10, min_y = 5, cnt_x, cnt_y;
float xO, yO;
float scale = 20;
int pointsPerStep = 2;

Pen *p;

float F1(float x) {
	return sin(x);
}

float F2(float x) {
	return cos(x);
}

float F3(float x) {
	return -cos(x);
}

float F4(float x) {
	return -sin(x);
}

void drawFunc(Window wnd, float (*func)(float), Pen *pen) {
	float x = 0, y = func(0), x_old = x, y_old = y;
	float dx = pointsPerStep / scale / num_step;
	do {
		x_old = x;
		y_old = y;
		x += dx;
		y = func(x);
		wnd.g->DrawLine(pen, xO + x_old * scale, yO - y_old * scale, xO + x * scale, yO - y * scale);
	} while (xO + x * scale <= wnd.Width());
	x = 0;
	y = func(0);
	do {
		x_old = x;
		y_old = y;
		x -= dx;
		y = func(x);
		wnd.g->DrawLine(pen, xO + x_old * scale, yO - y_old * scale, xO + x * scale, yO - y * scale);
	} while (xO + x * scale >= 0);
}

void drawAxis(Window wnd) {
	xO = wnd.Width() / 2;
	yO = wnd.Height() / 2;

	wnd.g->DrawLine(axisPen, 10., yO, wnd.Width() - 10., yO);
	wnd.g->DrawLine(axisPen, xO, 10., xO, wnd.Height() - 10.);
	axisArrow[0] = { xO, 10 };
	axisArrow[1] = { xO + 3, 20 };
	axisArrow[2] = { xO - 3, 20 };
	wnd.g->FillPolygon(axisBr, axisArrow, 3);
	axisArrow[0] = { wnd.Width() - 10, yO };
	axisArrow[1] = { wnd.Width() - 20, yO + 3 };
	axisArrow[2] = { wnd.Width() - 20, yO  - 3};
	wnd.g->FillPolygon(axisBr, axisArrow, 3);

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
	cnt_x = (wnd.Width() - 42) / 2 / px_step, cnt_y = (wnd.Height() - 42) / 2 / px_step;
	if (min_x % num_step == 0 && cnt_x * num_step < min_x) {
		cnt_x++;
	}
	if (min_y % num_step == 0 && cnt_y * num_step < min_y) {
		cnt_y++;
	}
	txtPos.Y = yO + 5;
	txtPos.Height = 10;
	txtPos.Width = 20;
	for (int i = 1; i <= cnt_x; i++) {
		wnd.g->DrawLine(axisPen, xO + px_step * i, yO - 4, xO + px_step * i, yO + 4);
		wnd.g->DrawLine(axisPen, xO - px_step * i, yO - 4, xO - px_step * i, yO + 4);
		txtPos.X = xO + px_step * i - 10;
		swprintf_s(text, 5, L"%d", i * num_step);
		wnd.g->DrawString(text, lstrlenW(text), font, txtPos, sf, axisBr);
		txtPos.X = xO - px_step * i - 10;
		swprintf_s(text, 5, L"%d", -i * num_step);
		wnd.g->DrawString(text, lstrlenW(text), font, txtPos, sf, axisBr);
	}
	txtPos.X = xO + 5;
	for (int i = 1; i <= cnt_y; i++) {
		wnd.g->DrawLine(axisPen, xO - 4, yO + px_step * i, xO + 4, yO + px_step * i);
		wnd.g->DrawLine(axisPen, xO - 4, yO - px_step * i, xO + 4, yO - px_step * i);
		txtPos.Y = yO + px_step * i - 5;
		swprintf_s(text, 5, L"%d", -i * num_step);
		wnd.g->DrawString(text, lstrlenW(text), font, txtPos, sf, axisBr);
		txtPos.Y = yO - px_step * i - 5;
		swprintf_s(text, 5, L"%d", i * num_step);
		wnd.g->DrawString(text, lstrlenW(text), font, txtPos, sf, axisBr);
	}
}

void resize(Window wnd) {
	scale = min((float)(wnd.Width() - 42) / 2 / min_x, (float)(wnd.Height() - 42) / 2 / min_y);
	drawAxis(wnd);
	drawFunc(wnd, F1, f1Pen);
	drawFunc(wnd, F2, f2Pen);
	drawFunc(wnd, F3, f3Pen);
	drawFunc(wnd, F4, f4Pen);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
	MSG Msg;
	Window myWindow(hInstance, nCmdShow, 300, 300, Color(255, 255, 255));
	p = new Pen(Color(255, 0, 0), 2);
	axisPen = new Pen(Color(0, 0, 0), 1);
	f1Pen = new Pen(Color(255, 0, 0), 2);
	f2Pen = new Pen(Color(0, 0, 255), 2);
	f3Pen = new Pen(Color(0, 128, 0), 2);
	f4Pen = new Pen(Color(255, 128, 0), 2);
	axisBr = new SolidBrush(Color(0, 0, 0));
	FontFamily fontFamily(L"Arial");
	font = new Font(&fontFamily, 9, FontStyleRegular, UnitPixel);
	sf = new StringFormat(0, 0);
	sf->SetAlignment(StringAlignmentCenter);

	myWindow.OnResizeFunction(resize);

	while (GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}