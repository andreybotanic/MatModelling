#include "window.h"
#include <string>
#include <iostream>
#include <fstream>

const float k = 1e9;
const float r0 = 1e5;

const float scale = 100;	// Scale for drawing
const int chargeR = 3;		// In this radius the potential will not calculate and the cycle will draw

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

Color HSL(float H, float S, float L) {
	float C = (1 - abs(2 * L - 1)) * S;
	float X = C * (1 - abs(int(H / 60.0) % 2 - 1));
	float m = L - C / 2;
	float R, G, B;
	if (H < 60) {
		R = C; G = X; B = 0;
	}
	else if (H < 120) {
		R = X; G = C; B = 0;
	}
	else if (H < 180) {
		R = 0; G = C; B = X;
	}
	else if (H < 240) {
		R = 0; G = X; B = C;
	}
	else if (H < 300) {
		R = X; G = 0; B = C;
	}
	else if (H < 360) {
		R = C; G = 0; B = X;
	}
	return Color((R + m) * 255, (G + m) * 255, (B + m) * 255);
}

float hue2rgb(float p, float q, float t) {
	if (t < 0) t += 1;
	if (t > 1) t -= 1;
	if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
	if (t < 1.0 / 2.0) return q;
	if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
	return p;
}

Color hslToRgb(float h, float s, float l) {
	float r, g, b;
	if (s == 0) {
		r = g = b = l; // achromatic
	}
	else {
		float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
		float p = 2 * l - q;
		r = hue2rgb(p, q, h + 1 / 3.0);
		g = hue2rgb(p, q, h);
		b = hue2rgb(p, q, h - 1 / 3.0);
	}
	return Color(round(r * 255), round(g * 255), round(b * 255));
}

inline int sign(float x) {
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
		float minP = 1e10;
		float maxP = -1e10;

		for (int x = 0; x < wnd.Width(); x++)
			for (int y = 0; y < wnd.Height(); y++) {
				phi = 0;
				for (int i = 0; i < charge_cnt; i++) {
					dx = charges[i].pos.X - x / scale;
					dy = charges[i].pos.Y - y / scale;
					d2 = dx*dx + dy*dy;
					if (d2 > chargeR*chargeR / (scale*scale)) {
						switch (charges[i].type) {
						case 1:
							phi += k * charges[i].value / sqrt(d2);
							break;
						case 2:
							phi += -2 * k * charges[i].value * log(sqrt(d2) / r0);
							break;
						}
					}
				}
				if (phi > maxP)
					maxP = phi;
				if (phi < minP)
					minP = phi;
			}

		for (int x = 0; x < wnd.Width(); x++)
			for (int y = 0; y < wnd.Height(); y++) {
				phi = 0;
				for (int i = 0; i < charge_cnt; i++) {
					dx = charges[i].pos.X - x / scale;
					dy = charges[i].pos.Y - y / scale;
					d2 = dx*dx + dy*dy;
					if (d2 > chargeR*chargeR / (scale*scale)) {
						switch (charges[i].type) {
						case 1:
							phi += k * charges[i].value / sqrt(d2);
							break;
						case 2:
							phi += -2 * k * charges[i].value * log(sqrt(d2) / r0);
							break;
						}
					}
				}
				//value = modf(phi / 10.0, &tmp);
				//v = 127.0 * (1.0 - cos(3.1415926 * value)*sign(value));
				//v = 127.0 * (1.0 - cos(3.1415926 * (phi - minP) / (maxP - minP))*sign(phi - minP));
				//bmp->SetPixel(x, y, Color(v, v, v));
				v = (1 - cos((phi - minP) * 3.1415926 / (maxP - minP))) / 2;
				bmp->SetPixel(x, y, hslToRgb(v, 1, 0.5));
			}
	}
	wnd.g->DrawImage(bmp, 0, 0);
	SolidBrush *b = new SolidBrush(Color::White);
	Pen *p = new Pen(Color::Black);
	for (int i = 0; i < charge_cnt; i++) {
		wnd.g->FillEllipse(b, int(charges[i].pos.X * scale) - chargeR, int(charges[i].pos.Y * scale) - chargeR, chargeR * 2, chargeR * 2);
		wnd.g->DrawEllipse(p, int(charges[i].pos.X * scale) - chargeR, int(charges[i].pos.Y * scale) - chargeR, chargeR * 2, chargeR * 2);
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