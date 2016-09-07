#include "window.h"

LRESULT CALLBACK Window::WindowProc(UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_SIZE:
			InitGDI();
			if (OnResize)
				(*OnResize)(*this, wndinfo);
			BitBlt(wndinfo.hdc, 0, 0, wndinfo.rt.right, wndinfo.rt.bottom, wndinfo.buffer_dc, 0, 0, SRCCOPY);
			break;
		case WM_CLOSE:
			DestroyWindow(wndinfo.hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(wndinfo.hwnd, msg, wParam, lParam);
	}
	return 0;
}

void Window::InitGDI() {
	wndinfo.hdc = GetDC(wndinfo.hwnd);
	wndinfo.buffer_dc = CreateCompatibleDC(wndinfo.hdc);
	GetClientRect(wndinfo.hwnd, &wndinfo.rt);
	wndinfo.width = wndinfo.rt.right;
	wndinfo.height = wndinfo.rt.bottom;
	wndinfo.bmp = CreateCompatibleBitmap(wndinfo.hdc, wndinfo.rt.right, wndinfo.rt.bottom);
	SelectObject(wndinfo.buffer_dc, wndinfo.bmp);
	SelectBrush(wndinfo.buffer_dc, brush);
	SelectPen(wndinfo.buffer_dc, pen);
}

LRESULT CALLBACK Window::StaticWindowProc(HWND hWnd_, UINT Msg_, WPARAM wParam_, LPARAM lParam_) {
	LONG_PTR user_data = GetWindowLongPtr(hWnd_, GWLP_USERDATA);
	Window * this_window = reinterpret_cast<Window *>(user_data);
	return this_window->WindowProc( Msg_, wParam_, lParam_);
}

LRESULT CALLBACK Window::InitialWindowProc(HWND hWnd_, UINT Msg_, WPARAM wParam_, LPARAM lParam_) {
	if (Msg_ == WM_NCCREATE) {
		LPCREATESTRUCT create_struct = reinterpret_cast<LPCREATESTRUCT>(lParam_);
		void * lpCreateParam = create_struct->lpCreateParams;
		Window * this_window = reinterpret_cast<Window *>(lpCreateParam);
		this_window->wndinfo.hwnd = hWnd_;
		SetWindowLongPtr(hWnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this_window));
		SetWindowLongPtr(hWnd_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::StaticWindowProc));
		return this_window->WindowProc(Msg_, wParam_, lParam_);
	}
	return DefWindowProc(hWnd_, Msg_, wParam_, lParam_);
}

Window::Window(HINSTANCE hInstance, int nCmdShow, int width, int height, char className[]) {
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = &Window::InitialWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = className;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	wndinfo.hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		className,
		className,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height,
		NULL, NULL, hInstance, this);
	if (wndinfo.hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	ShowWindow(wndinfo.hwnd, nCmdShow);
	UpdateWindow(wndinfo.hwnd);
}

void Window::OnResizeFunction(void(*_OnResize)(Window, WNDINFO)) {
	OnResize = _OnResize;
	if (OnResize)
		(*OnResize)(*this, wndinfo);
	BitBlt(wndinfo.hdc, 0, 0, wndinfo.rt.right, wndinfo.rt.bottom, wndinfo.buffer_dc, 0, 0, SRCCOPY);
}

void Window::SetPen(HPEN _pen) {
	pen = _pen;
	SelectPen(wndinfo.buffer_dc, pen);
}

HPEN Window::GetPen() {
	return pen;
}

void Window::SetBrush(HBRUSH _brush) {
	brush = _brush;
	SelectBrush(wndinfo.buffer_dc, brush);
}

HBRUSH Window::GetBrush() {
	return brush;
}

void Window::SetBackground(COLORREF _color) {
	color = _color;
	back = CreateSolidBrush(color);
}

COLORREF Window::GetBackground() {
	return color;
}

void Window::Clear() {
	SelectBrush(wndinfo.buffer_dc, back);
	PatBlt(wndinfo.buffer_dc, 0, 0, wndinfo.rt.right, wndinfo.rt.bottom, PATCOPY);
	SelectBrush(wndinfo.buffer_dc, brush);
}
