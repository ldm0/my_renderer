#include"scg_ext.h"
#include"scg.h"
#include<math.h>    // for sqrt

void scg_draw_line(scg_color color, int x1, int y1, int x2, int y2)
{
    int x_max, x_min;
    int y_max, y_min;
    if (x1 > x2) {
        x_max = x1;
        x_min = x2;
    } else {
        x_max = x2;
        x_min = x1;
    }
    if (y1 > y2) {
        y_max = y1;
        y_min = y2;
    } else {
        y_max = y2;
        y_min = y1;
    }
	x_min = min(max(x_min , 0), scg_window_width - 1);
	x_max = min(max(x_max , 0), scg_window_width - 1);
	y_min = min(max(y_min , 0), scg_window_height - 1);
	y_max = min(max(y_max , 0), scg_window_height - 1);

    int tmp = x2 * y1 - x1 * y2;
    int dx = x2 - x1;
    int dy = y2 - y1;
    if (abs(dx) >= abs(dy)) {
        for (int x = x_min; x < x_max; ++x) {
            int y = (dy * x + tmp) / dx;
			y = min(max(y , 0), scg_window_height - 1);
            scg_back_buffer[x + y * scg_window_width] = color;
        }
    } else {
        for (int y = y_min; y < y_max; ++y) {
            int x = (dx * y - tmp) / dy;
			x = min(max(x , 0), scg_window_width - 1);
            scg_back_buffer[x + y * scg_window_width] = color;
        }
    }
}

void scg_draw_circle(scg_color color, int _x, int _y, double radius)
{
	// draw from left to right
    int x_min = (int)(_x - radius);
    int x_max = (int)(_x + radius);
	x_min = min(max(x_min, 0), scg_window_width - 1);
	x_max = min(max(x_max, 0), scg_window_width - 1);
    for (int x = x_min > 0 ? (int)x_min: 0; x < x_max; ++x) {
        double dx = (double)x - (double)_x;
        double dy = sqrt(radius * radius - dx * dx);
        int y1 = (int)(_y - dy);
        int y2 = (int)(_y + dy);
        if (y1 >= 0 && y1 < scg_window_height) {
            scg_back_buffer[x + y1 * scg_window_width] = color;
        }
        if (y2 >= 0 && y2 < scg_window_height) {
            scg_back_buffer[x + y2 * scg_window_width] = color;
        }
    }

	// draw from top to bottom
    int y_min = (int)(_y - radius);
    int y_max = (int)(_y + radius);
	y_min = min(max(y_min, 0), scg_window_height - 1);
	y_max = min(max(y_max, 0), scg_window_height - 1);
    for (int y = y_min > 0 ? (int)y_min: 0; y < y_max; ++y) {
        double dy = (double)y - (double)_y;
        double dx = sqrt(radius * radius - dy * dy);
        int x1 = (int)(_x - dx);
        int x2 = (int)(_x + dx);
        if (x1 >= 0 && x1 < scg_window_width) {
            scg_back_buffer[x1 + y * scg_window_width] = color;
        }
        if (x2 >= 0 && x2 < scg_window_width) {
            scg_back_buffer[x2 + y * scg_window_width] = color;
        }
    }
}