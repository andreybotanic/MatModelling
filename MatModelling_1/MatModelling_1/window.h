#pragma once

#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")
using namespace Gdiplus;

struct WNDINFO {
	HWND hwnd;
	HDC hdc;
	HDC buffer_dc;
	RECT rt;
	HBITMAP bmp;
	LONG width, height;
};

class Window {
private:
	HWND hwnd;
	RECT rt;
	LONG width, height;
	Gdiplus::Color color;
	ULONG_PTR gdiplusToken;
	bool isCreated = false;
	Bitmap *bmp = new Bitmap(0, 0);
	Gdiplus::Graphics *g_buffer;
	UINT_PTR MY_TIMER;
	int interval = 100;
	//
	void (*OnResize)(Window wnd) = NULL;
	void (*OnTimerTick)(Window wnd) = NULL;
	//
	static LRESULT CALLBACK StaticWindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK InitialWindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
	LRESULT CALLBACK WindowProc(UINT Msg, WPARAM wParam, LPARAM lParam);
	void ReinitGDI();
public:
	Gdiplus::Graphics *g;
	Window(HINSTANCE hInstance, int nCmdShow, int width, int height, Gdiplus::Color color, char className[] = "myWindowClass");
	void OnResizeFunction(void(*_OnResize)(Window));
	void OnTimerTickFunction(void(*_OnTimerTick)(Window), int i);
	void SetInterval(int i);
	void TimerStop();
	void ShowMessage(char* title, char* msg);
	void SetBackground(Color _color);
	Color GetBackground();
	inline REAL Width() { return width; };
	inline REAL Height() { return height; };
};