#pragma once

#ifndef _SCG_EXT_H_
#define _SCG_EXT_H_

typedef unsigned scg_color;
#define SCG_GET_RED(x) (x & 0xFF)
#define SCG_GET_GREEN(x) ((x >> 8) & 0xFF)
#define SCG_GET_BLUE(x) ((x >> 16) & 0xFF)

// Assume upper left corner is origin
void scg_draw_line(scg_color color, int x1, int y1, int x2, int y2);

void scg_draw_circle(scg_color color, int x, int y, double radius);

#endif