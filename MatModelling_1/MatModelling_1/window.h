#pragma once

#include <windows.h>
#include <windowsx.h>

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
	WNDINFO wndinfo;
	HPEN pen;
	HBRUSH brush;
	HBRUSH back;
	COLORREF color;
	//
	void (*OnResize)(Window wnd, WNDINFO _wndinfo) = NULL;
	//
	static LRESULT CALLBACK StaticWindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK InitialWindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
	LRESULT CALLBACK WindowProc(UINT Msg, WPARAM wParam, LPARAM lParam);
	void InitGDI();
public:
	Window(HINSTANCE hInstance, int nCmdShow, int width, int height, char className[] = "myWindowClass");
	void OnResizeFunction(void(*_OnResize)(Window, WNDINFO));
	void SetPen(HPEN _pen);
	HPEN GetPen();
	void SetBrush(HBRUSH _brush);
	HBRUSH GetBrush();
	void SetBackground(COLORREF _color);
	COLORREF GetBackground();
	void Clear();
};