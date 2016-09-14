#include "window.h"
#include <string>

const int turtlesCount = 20;
float speed = 5;
float scale = 1;
PointF turtles[turtlesCount];
Pen *tPen;
SolidBrush *tBr;
int iteration = 0;
float maxD = 0;

Font *font;
StringFormat *sf;
RectF txtPos;
WCHAR text[100];

float distance(PointF p1, PointF p2) {
	return sqrt((p2.X - p1.X)*(p2.X - p1.X) + (p2.Y - p1.Y)*(p2.Y - p1.Y));
}

PointF normalize(PointF vec) {
	float l = sqrt(vec.X*vec.X + vec.Y*vec.Y);
	return PointF(vec.X / l, vec.Y / l);
}

inline PointF mult(PointF vec, float l) {
	return PointF(vec.X * l, vec.Y * l);
}

inline PointF add(PointF vec1, PointF vec2) {
	return PointF(vec1.X + vec2.X, vec1.Y + vec2.Y);
}

void initRandTurtles(Window wnd) {
	for (int i = 0; i < turtlesCount; i++) {
		turtles[i].X = rand() % (int)wnd.Width();
		turtles[i].Y = rand() % (int)wnd.Height();
	}
}

void initPegularTurtles(Window wnd) {
	PointF c;
	c.X = wnd.Width() / 2;
	c.Y = wnd.Height() / 2;
	float R = min(c.X, c.Y) - 10;
	float step = 2 * 3.1415926 / turtlesCount, phi = 0;
	for (int i = 0; i < turtlesCount; i++, phi += step) {
		turtles[i].X = R*sin(phi) + c.X;
		turtles[i].Y = R*cos(phi) + c.Y;
	}
}

void drawTurtles(Window wnd) {
	for (int i = 0; i < turtlesCount; i++) {
		wnd.g->FillEllipse(tBr, turtles[i].X - 2, turtles[i].Y - 2, 5., 5.);
		wnd.g->DrawEllipse(tPen, turtles[i].X - 2, turtles[i].Y - 2, 5., 5.);
	}
}

void step(Window wnd) {
	PointF vec, deltha;
	iteration++;
	for (int i = 0; i < turtlesCount; i++) {
		vec.X = turtles[(i + 1) % turtlesCount].X - turtles[i].X;
		vec.Y = turtles[(i + 1) % turtlesCount].Y - turtles[i].Y;
		deltha = mult(normalize(vec), speed);
		turtles[i] = add(turtles[i], deltha);
	}
	float d;
	maxD = 0;
	for (int i = 0; i < turtlesCount; i++) {
		d = distance(turtles[i], turtles[(i + 1) % turtlesCount]);
		if (d > maxD)
			maxD = d;
	}
	if (abs(maxD - speed) < 1e-5) {
		wnd.TimerStop();
		wnd.ShowMessage("", "Simulation complete!");
	}
}

void printInfo(Window wnd) {
	txtPos.X = 0;
	txtPos.Y = 0;
	txtPos.Width = 150;
	txtPos.Height = 70;
	swprintf_s(text, 100, L"Count: %d\nSpeed: %f\nIteration: %d\nMax dist: %f", turtlesCount, speed, iteration, maxD);
	wnd.g->DrawString(text, lstrlenW(text), font, txtPos, sf, &SolidBrush(Color::Black));
}

void resize(Window wnd) {
	wnd.g->Clear(Color::White);
	drawTurtles(wnd);
	printInfo(wnd);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
	MSG Msg;
	Window myWindow(hInstance, nCmdShow, 500, 500, Color(255, 255, 255));
	tPen = new Pen(Color(192, 64, 0), 1.5);
	tBr = new SolidBrush(Color(0, 128, 0));

	initPegularTurtles(myWindow);
	//initRandTurtles(myWindow);

	FontFamily fontFamily(L"Arial");
	font = new Font(&fontFamily, 9, FontStyleRegular, UnitPixel);
	sf = new StringFormat(0, 0);
	sf->SetAlignment(StringAlignmentNear);

	myWindow.OnResizeFunction(resize);
	myWindow.OnTimerTickFunction(step, 20);

	while (GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}