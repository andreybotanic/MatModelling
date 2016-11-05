#include "window.h"
#include <string>

//#define TEST

Pen *axisPen, *f1Pen;
SolidBrush *axisBr;
PointF axisArrow[3];
Font *font;
StringFormat *sf;
RectF txtPos;
WCHAR text[10];
int num_step;
#ifdef TEST
float min_x = 3, min_y = 3;
#else
float min_x = 1.5, min_y = 0.6;
#endif


int cnt_x, cnt_y;
float xO, yO;
float scale = 20;
int pointsPerStep = 2;

Pen *p;

double sigma = 7.595e-19;
double a = 0.66074e-10;
double r0 = 0.74160e-10;
double m = 1.673564e-27;
double pi = 3.1415926;
double plank = 1.0545887e-34;
#ifdef TEST
double gamma = 1 / 3.1415926;
#else
double gamma = (a * sqrt(m * sigma)) / (pi*plank);
#endif

double func(double x, double eps) {
#ifdef TEST
	return sqrt(eps - 0.25 * x*x);
#else
	return sqrt(eps - 4 * (pow(x, -12) - pow(x, -6)));
#endif;
}

struct energy {
	double E;
	double x1;
	double x2;
};

energy en[10];

double Simpson(float left, float right, double eps) {
	int cnt = 10001;
	double tmp;
	if (left > right) {
		tmp = left;
		left = right;
		right = tmp;
	}
	double step = (right - left) / cnt;
	double x = left + step;
	double I = 0;//func(left, eps) + func(right, eps);
	for (int i = 1; i < cnt; i++) {
		if (i % 2)
			I += 4 * func(x, eps);
		else
			I += 2 * func(x, eps);
		x += step;
	}
	I /= 3;
	I *= step;
	return I;
}

double func2(double eps, int n) {
	double x1, x2, z1, z2;
#ifdef TEST
	x1 = 2 * sqrt(eps);
	x2 = -2 * sqrt(eps);
#else
	z1 = 0.5 - sqrt(1 + eps) / 2.0;
	z2 = 0.5 + sqrt(1 + eps) / 2.0;
	x1 = 1 / pow(z1, 1 / 6.0);
	x2 = 1 / pow(z2, 1 / 6.0);
#endif
	en[n].x1 = x1;
	en[n].x2 = x2;
	return gamma * Simpson(x1, x2, eps) - n - 0.5;
}



void drawFunc(Window wnd, float(*func)(float), Pen *pen) {
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

void solve(Window wnd) {
	double error = 1e-3;
	double z1, z2, x1, x2;
	double eps;
	double left, right, mid, fm, fl, fr;
#ifdef TEST
	left = 1e-3;
	right = 5;
#else
	left = -1e-3;
	right = -1 + 1e-3;
#endif
	mid = (left + right) / 2;
	int n = 5;// func2(left, 0);
	for (int i = 0; i < n; i++) {
		fl = func2(left, i);
		fr = func2(right, i);
		fm = func2(mid, i);
		do {
			if (fm*fr < 0) {
				left = mid;
				fl = fm;
			}
			else {
				right = mid;
				fr = fm;
			}
			mid = (left + right) / 2;
			fm = func2(mid, i);
		} while (abs(fm) > error);
		en[i].E = mid;
#ifdef TEST
		left = 1e-3;
		right = 5;
#else
		left = -1e-3;
		right = -1 + 1e-3;
#endif
		wnd.g->DrawLine(p, int(en[i].x2*scale) + xO, yO - mid*scale, int(en[i].x1 *scale) + xO, yO - mid*scale );
		txtPos.X = en[i].x1*scale + xO + 5;
		txtPos.Y = yO - mid*scale;
		txtPos.Height = 10;
		txtPos.Width = 30;
		swprintf_s(text, 10, L"%.3lf", en[i].E);
		wnd.g->DrawString(text, lstrlenW(text), font, txtPos, sf, axisBr);
	}
}

float F1(float x) {
#ifdef TEST
	return 0.25 * x*x;
#else
	return 4 * (pow(1 / x, 12) - pow(1 / x, 6));
#endif
}

void drawAxis(Window wnd) {
#ifdef TEST
	xO = wnd.Width() / 2;
	yO = wnd.Height() - 30;
#else
	xO = 20;
	yO = 30;
#endif

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
	cnt_x = (wnd.Width() - 42) / px_step, cnt_y = (wnd.Height() - 42) / px_step;
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
	solve(wnd);
	drawFunc(wnd, F1, f1Pen);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
	MSG Msg;
	Window myWindow(hInstance, nCmdShow, 300, 300, Color(255, 255, 255));
	p = new Pen(Color(255, 0, 0), 2);
	axisPen = new Pen(Color(0, 0, 0), 1);
	f1Pen = new Pen(Color(0, 0, 255), 2);
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