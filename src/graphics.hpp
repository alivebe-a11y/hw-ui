#pragma once

#if defined(_WIN32)
//#define WINDOWS_GRAPHICS
#endif // WINDOWS
#define WINDOW_NAME "hw-smi"
//#define GRAPHICS_CONSOLE
#define FONT_HEIGHT 10 // default: 10
#define FONT_WIDTH 5 // default: 6

#include "utilities.hpp"
#include <atomic>

extern std::atomic_bool running;

#ifdef WINDOWS_GRAPHICS
void main_graphics();
void main_physics();

struct Camera {
	uint width = 700u; // screen width
	uint height = 150u; // screen height
	uint fps_limit = 60u; // default value for screen frames per second limit
};
extern Camera camera;

struct Color {
	uchar r, g, b;
	Color(const int r, const int g, const int b) {
		this->r = (uchar)r;
		this->g = (uchar)g;
		this->b = (uchar)b;
	}
	Color() = default;
};

void draw_pixel(const Color& c, const int x, const int y); // 2D drawing functions
void draw_circle(const Color& c, const int x, const int y, const int r);
void draw_line(const Color& c, const int x0, const int y0, const int x1, const int y1);
void draw_triangle(const Color& c, const int x0, const int y0, const int x1, const int y1, const int x2, const int y2);
void draw_rectangle(const Color& c, const int x0, const int y0, const int x1, const int y1);
void draw_polygon(const Color& c, const int* const x, const int* const y, const int n);
void draw_text(const Color& c, const string& s, const int x, const int y);
void draw_bitmap(const void* buffer);
#endif // WINDOWS_GRAPHICS