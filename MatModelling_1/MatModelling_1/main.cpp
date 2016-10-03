#include "window.h"
#include <string>
#include <iostream>
#include <fstream>

const float k = 1e9;
const float r0 = 1e5;

struct Charge {
	PointF pos;
	float value;
	int type;
	Charge() {};
	Charge(int t, float x, float y, float v) : type(t), pos(x, y), value(v) {};
};

Charge *charges;
int charge_cnt = 0;
bool computed = false;
Bitmap *bmp;

int sign(float x) {
	return x > 0 ? 1 : -1;
}

void resize(Window wnd) {
//	wnd.g->Clear(Color::White);
	if (!computed) {
		computed = true;
		bmp = new Bitmap(wnd.Width(), wnd.Height());
		float phi;
		float d2;
		float dx, dy;
		float value, v;
		float tmp;
		for (int x = 0; x < wnd.Width(); x++)
			for (int y = 0; y < wnd.Height(); y++) {
				phi = 0;
				for (int i = 0; i < charge_cnt; i++) {
					dx = charges[i].pos.X - x / 100.0;
					dy = charges[i].pos.Y - y / 100.0;
					d2 = dx*dx + dy*dy;
					switch (charges[i].type) {
					case 1:
						phi += k * charges[i].value / sqrt(d2);
						break;
					case 2:
						phi += -2 * k * charges[i].value * log(sqrt(d2) / r0);
						break;
					}
				}
				value = modf(phi / 10.0, &tmp);
				v = 127.0 * (1.0 - cos(3.1415926 * value)*sign(value));
				bmp->SetPixel(x, y, Color(v, v, v));
			}
	}
	wnd.g->DrawImage(bmp, 0, 0);
	SolidBrush *b = new SolidBrush(Color::Blue);
	Pen *p = new Pen(Color::Black);
	for (int i = 0; i < charge_cnt; i++) {
		wnd.g->FillEllipse(b, int(charges[i].pos.X * 100) - 3, int(charges[i].pos.Y * 100) - 3, 6, 6);
		wnd.g->DrawEllipse(p, int(charges[i].pos.X * 100) - 3, int(charges[i].pos.Y * 100) - 3, 6, 6);
	}
}

void init(Window wnd, char* file) {
	std::ifstream fin;
	fin.open(file);
	float x, y, v;
	fin >> charge_cnt;
	int t;
	charges = new Charge[charge_cnt];
	for (int i = 0; i < charge_cnt; i++) {
		fin >> t >> x >> y >> v;
		charges[i] = Charge(t, x, y, v);
	}
}

int ParseCommandline(char ***argv)
{
	int    argc, BuffSize, i;
	WCHAR  *wcCommandLine;
	LPWSTR *argw;
	wcCommandLine = GetCommandLineW();
	argw = CommandLineToArgvW(wcCommandLine, &argc);
	*argv = (char **)GlobalAlloc(LPTR, argc + 1);
	for (i = 0; i < argc; i++)
	{
		BuffSize = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, argw[i], -1, NULL, 0, NULL, NULL);
		(*argv)[i] = (char *)GlobalAlloc(LPTR, BuffSize);
		WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, argw[i], BuffSize * sizeof(WCHAR), (*argv)[i], BuffSize, NULL, NULL);
	}
	return argc;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
	MSG Msg;
	Window myWindow(hInstance, nCmdShow, 1000, 600, Color::Black);
	char **argv = NULL;
	ParseCommandline(&argv);
	init(myWindow, argv[1]);
	myWindow.OnResizeFunction(resize);
	while (GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}