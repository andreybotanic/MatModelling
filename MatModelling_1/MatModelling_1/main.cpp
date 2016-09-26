#include "window.h"
#include <string>
#include <time.h>

const float G = 9.8;			// m/s^2

class Salut {
private:
	float scale;
	bool isBang = false;
	float start_time;
	float bang_time;
	float T = 0;
	float dT = 0.05;
	float bang_h = 220;
	PointF start_pos, current_pos, start_velocity;
	PointF splinters_pos[400], splinters_start_velocity[400], splinters_current_velocity[400], splinters_start_pos[400];
	float life_time[400];
	Pen *p;
	SolidBrush *b;
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
	void bang() {
		bang_time = T;
		for (int i = 0; i < 400; i++) {
			float phi = rand() % 360 * 3.1415926 / 180;
			float r = (rand() % 500) / 10.0;
			splinters_start_velocity[i] = { r*cos(phi) + start_velocity.X, r*sin(phi) + start_velocity.Y - G*(T - start_time) };
			splinters_start_pos[i] = current_pos;
			life_time[i] = (rand() % 30) / 10.0 + 1;
		}
	}
public:
	Salut(Window wnd, float pause = 0) {
		scale = 3 / 5.0 * wnd.Height() / bang_h;
		bang_h -= (rand() % 400) / 10.0;
		p = new Pen(Color::White, 1);
		b = new SolidBrush(Color(HSL(rand() % 360, (rand() % 30) / 100.0 + 0.7, (rand() % 40) / 100.0 + 0.3)));
		start_pos = { (rand() % int(wnd.Width())) / scale, 0 };
		float cx = (wnd.Width() / 2) / scale;
		start_velocity = { REAL((cx - start_pos.X) / 20.0), REAL(sqrt(bang_h * 2 * G) + (rand() % 100) / 10.0 + 5.0) };
		start_time = pause;
	}
	void step() {
		if (T >= start_time) {
			if (current_pos.Y < bang_h)
				current_pos = { start_pos.X + start_velocity.X*(T-start_time), start_pos.Y + start_velocity.Y * (T - start_time) - G*(T - start_time)*(T - start_time) / 2 };
			else {
				if (!isBang) {
					isBang = true;
					bang();
				}
				else
					for (int i = 0; i < 400; i++)
						splinters_pos[i] = { splinters_start_pos[i].X + splinters_start_velocity[i].X * (T - bang_time), splinters_start_pos[i].Y + splinters_start_velocity[i].Y * (T - bang_time) - G*(T - bang_time)*(T - bang_time) / 2 };
			}
		}
		T += dT;
	}
	void draw(Window wnd) {
		if (T >= start_time) {
			if (isBang) {
				for (int i = 0; i < 400; i++)
					if (T - bang_time < life_time[i])
						wnd.g->FillEllipse(b, splinters_pos[i].X * scale - 2 / 2.0, wnd.Height() - splinters_pos[i].Y * scale - 2 / 2.0, 2, 2);
			}
			else {
				wnd.g->FillEllipse(b, current_pos.X * scale - 5 / 2.0, wnd.Height() - current_pos.Y * scale - 5 / 2.0, 5, 5);
				wnd.g->DrawEllipse(p, current_pos.X * scale - 5 / 2.0, wnd.Height() - current_pos.Y * scale - 5 / 2.0, 5, 5);
			}
		}
	}
};

const int salut_num = 100;
Salut *saluts[salut_num];

void step(Window wnd) {
	for (int i = 0; i < salut_num; i++) {
		saluts[i]->step();
	}
}

void resize(Window wnd) {
	wnd.g->Clear(Color::Black);
	for (int i = 0; i < salut_num; i++) {
		saluts[i]->draw(wnd);
	}
}

void init(Window wnd) {
	srand(time(0));
	for (int i = 0; i < salut_num; i++) {
		saluts[i] = new Salut(wnd, (rand() % salut_num * 10) / 50.0);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
	MSG Msg;
	Window myWindow(hInstance, nCmdShow, 1000, 600, Color::Black);
	init(myWindow);
	myWindow.OnResizeFunction(resize);
	myWindow.OnTimerTickFunction(step, 50);
	while (GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}