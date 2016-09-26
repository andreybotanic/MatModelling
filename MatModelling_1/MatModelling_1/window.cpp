#include "window.h"

Window::Window(HINSTANCE hInstance, int nCmdShow, int width, int height, Gdiplus::Color color, char className[]){
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
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		className,
		className,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height,
		NULL, NULL, hInstance, this);
	if (hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&(gdiplusToken), &gdiplusStartupInput, NULL);
	this->color = color;
	isCreated = true;
	ReinitGDI();
}

LRESULT CALLBACK Window::WindowProc(UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE:
			SetTimer(hwnd, MY_TIMER, interval, NULL);
			break;
		case WM_TIMER:
			if (wParam == MY_TIMER) {
				if (OnTimerTick) {
					(*OnTimerTick)(*this);
				}
				if (OnResize) {
					(*OnResize)(*this);
					g_buffer->DrawImage(bmp, 0, 0, width, height);
				}
			}
			break;
		case WM_SIZE:
			ReinitGDI();
			if (OnResize) {
				(*OnResize)(*this);
				g_buffer->DrawImage(bmp, 0, 0, width, height);
			}
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

void Window::ReinitGDI() {
	if (!isCreated)
		return;
	delete bmp;
	delete g_buffer;
	GetClientRect(hwnd, &rt);
	width = rt.right;
	height = rt.bottom;
	bmp = new Bitmap(width, height);
	g = Graphics::FromImage(bmp);
	g_buffer = new Gdiplus::Graphics(hwnd, true);
	g->SetSmoothingMode(SmoothingModeAntiAlias);
	g->Clear(Color(this->color));
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
		this_window->hwnd = hWnd_;
		SetWindowLongPtr(hWnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this_window));
		SetWindowLongPtr(hWnd_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::StaticWindowProc));
		return this_window->WindowProc(Msg_, wParam_, lParam_);
	}
	return DefWindowProc(hWnd_, Msg_, wParam_, lParam_);
}

void Window::OnResizeFunction(void(*_OnResize)(Window)) {
	OnResize = _OnResize;
	if (OnResize) {
		(*OnResize)(*this);
		g_buffer->DrawImage(bmp, 0, 0, width, height);
	}
}

void Window::OnTimerTickFunction(void(*_OnTimerTick)(Window), int i) {
	OnTimerTick = _OnTimerTick;
	SetTimer(hwnd, MY_TIMER, i, NULL);
}

void Window::SetInterval(int i) {
	SetTimer(hwnd, MY_TIMER, i, NULL);
}

void Window::TimerStop() {
	KillTimer(hwnd, MY_TIMER);
}

void Window::ShowMessage(char * title, char * msg) {
	MessageBox(hwnd, msg, title, 0);
}

void Window::SetBackground(Color _color) {
	color = _color;
}

Color Window::GetBackground() {
	return color;
}