#include "graphics.hpp"

std::atomic_bool running = true;

#ifdef WINDOWS_GRAPHICS
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>
HWND hWnd;
HDC frontDC, backDC;
HBITMAP frame;

Camera camera;

void draw_pixel(const Color& c, const int x, const int y) {
	SetPixel(backDC, x, y, RGB(c.r, c.g, c.b));
}
void draw_circle(const Color& c, const int x, const int y, const int r) {
	if(r<2) {
		SetPixel(backDC, x, y, RGB(c.r, c.g, c.b));
	} else {
		SelectObject(backDC, GetStockObject(NULL_BRUSH));
		SetDCPenColor(backDC, RGB(c.r, c.g, c.b));
		Ellipse(backDC, x-r, y-r, x+r, y+r);
		SelectObject(backDC, GetStockObject(DC_BRUSH));
	}
}
void draw_line(const Color& c, const int x0, const int y0, const int x1, const int y1) {
	SetDCPenColor(backDC, RGB(c.r, c.g, c.b));
	MoveToEx(backDC, x0, y0, NULL);
	LineTo(backDC, x1, y1);
}
void draw_triangle(const Color& c, const int x0, const int y0, const int x1, const int y1, const int x2, const int y2) {
	POINT p[3];
	p[0].x = x0; p[0].y = y0;
	p[1].x = x1; p[1].y = y1;
	p[2].x = x2; p[2].y = y2;
	SetDCPenColor(backDC, RGB(c.r, c.g, c.b));
	SetDCBrushColor(backDC, RGB(c.r, c.g, c.b));
	Polygon(backDC, p, 3);
}
void draw_rectangle(const Color& c, const int x0, const int y0, const int x1, const int y1) {
	SetDCPenColor(backDC, RGB(c.r, c.g, c.b));
	SetDCBrushColor(backDC, RGB(c.r, c.g, c.b));
	Rectangle(backDC, x0, y0, x1, y1);
}
void draw_polygon(const Color& c, const int* const x, const int* const y, const int n) {
	POINT* p = new POINT[n];
	for(int i=0; i<n; i++) {
		p[i].x = x[i];
		p[i].y = y[i];
	}
	SetDCPenColor(backDC, RGB(c.r, c.g, c.b));
	SetDCBrushColor(backDC, RGB(c.r, c.g, c.b));
	Polygon(backDC, p, n);
	delete[] p;
}
void draw_text(const Color& c, const string& s, const int x, const int y) {
	SetTextColor(backDC, RGB(c.r, c.g, c.b));
	TextOut(backDC, x, y, s.c_str(), (int)s.length());
}
void draw_bitmap(const void* buffer) {
	SetBitmapBits(frame, 4*camera.width*camera.height, buffer);
}

void update_frame() {
	BitBlt(frontDC, 0, 0, camera.width, camera.height, backDC, 0, 0, SRCCOPY); // copy back buffer to front buffer
	HPEN   oldPen   = (HPEN  )SelectObject(backDC, GetStockObject(BLACK_PEN  ));
	HBRUSH oldBrush = (HBRUSH)SelectObject(backDC, GetStockObject(BLACK_BRUSH));
	Rectangle(backDC, 0, 0, camera.width, camera.height); // clear back buffer
	SelectObject(backDC, oldPen);
	SelectObject(backDC, oldBrush);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
		case WM_NCHITTEST: // make window draggable
			{
				const LRESULT result = ::DefWindowProc(hWnd, message, wParam, lParam); // call the default window procedure for default handling
				return result==HTCLIENT ? HTCAPTION : result;
			}
		case WM_DESTROY:
			running = false;
			PostQuitMessage(0);
			exit(0);
			return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
#ifdef GRAPHICS_CONSOLE
int main(int argc, char* argv[]) { // call WinMain from dummy main function in order to have an additional console window
	const vector<string> main_arguments = get_main_arguments(argc, argv);
	return WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWMINIMIZED);
}
#endif // GRAPHICS_CONSOLE
INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PSTR, _In_ INT) {
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW|CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("WindowClass");
	RegisterClass(&wndClass);
	HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = {sizeof(mi)};
	if(!GetMonitorInfo(hMon, &mi)) return 1;
	DEVMODE lpDevMode; // get monitor fps
	memset(&lpDevMode, 0, sizeof(DEVMODE));
	camera.fps_limit = (uint)EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &lpDevMode)!=0 ? (uint)lpDevMode.dmDisplayFrequency : 60u; // find out screen refresh rate
	const uint screen_width  = (uint)(mi.rcMonitor.right-mi.rcMonitor.left); // get screen size, initialize variables
	const uint screen_height = (uint)(mi.rcMonitor.bottom-mi.rcMonitor.top);
	hWnd = CreateWindow("WindowClass", WINDOW_NAME, WS_POPUP|WS_VISIBLE, mi.rcMonitor.left, mi.rcMonitor.top, camera.width, camera.height, NULL, NULL, hInstance, 0); // create fullscreen window
	frontDC = GetDC(hWnd);
	frame = CreateCompatibleBitmap(frontDC, camera.width, camera.height); // initialize back buffer
	backDC = CreateCompatibleDC(frontDC);
	HBITMAP oldBMP = (HBITMAP)SelectObject(backDC, frame);
	DeleteObject(oldBMP);
	SelectObject(backDC, GetStockObject(DC_PEN  ));
	SelectObject(backDC, GetStockObject(DC_BRUSH));
	//SelectObject(backDC, GetStockObject(NULL_BRUSH)); for no filling of circles and polygons
	SetDCPenColor(backDC, RGB(255, 255, 255)); // define drawing properties
	SetDCBrushColor(backDC, RGB(0, 0, 0));
	SetTextAlign(backDC, TA_TOP);
	SetBkMode(backDC, TRANSPARENT);
	SetPolyFillMode(backDC, ALTERNATE);
	HFONT hFont = CreateFont(FONT_HEIGHT+5, FONT_WIDTH+1, 0, 0, 500, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, "Courier New"); // (HFONT)GetStockObject(ANSI_FIXED_FONT);
	SelectObject(backDC, hFont);
	const uint offset_x = screen_width-camera.width-5u-250u;
	const uint offset_y = 5u;
	SetWindowPos(hWnd, HWND_TOPMOST, offset_x, offset_y, camera.width, camera.height, SWP_DRAWFRAME|SWP_NOSIZE|SWP_SHOWWINDOW); // window stays in foreground
	int transparency = 255; // make window semi-transparent
	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE)|WS_EX_LAYERED);
	SetLayeredWindowAttributes(hWnd, 0, transparency, LWA_ALPHA|LWA_COLORKEY); // everything is semi-transparent | background is fully transparent
	thread compute_thread(main_physics); // start main_physics() in a new thread
	MSG msg = {};
	Clock clock;
	double frametime = 1.0;
	while(msg.message!=WM_QUIT) {
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message==WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// main loop ################################################################
		main_graphics();
		update_frame();
		frametime = clock.stop();
		sleep(1.0/(double)camera.fps_limit-frametime);
		clock.start();
		// ##########################################################################
	}
	ReleaseDC(hWnd, frontDC);
	compute_thread.join();
	return 0;
}
#endif // WINDOWS_GRAPHICS