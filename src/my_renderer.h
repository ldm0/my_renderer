#pragma once

#ifndef _MY_RENDERER_H_
#define _MY_RENDERER_H_

#include"my_graphic.h"
#include"my_obj_loader/my_obj_loader.h"
#include<Windows.h>
#include<tchar.h>

// Remember don't use draw before clear, because result will be overwrite
class Renderer {
public:
	enum DRAW_MODE{
		DRAW_MESH,
		DRAW_COLOR,
		DRAW_MESH_COLOR,
	} draw_mode;

#pragma region camera
	// initial direction is (0, 0, 1)
	// Assume left hand coordinates
	// yaw: y, pitch: x, roll: z
	vec3 yaw_pitch_roll;
	vec4 camera_position;
#pragma endregion

private:
#pragma region buffers
	unsigned* back_buffer;
	float *z_buffer;
#pragma endregion

#pragma region mesh
	my_obj_elements *mesh;
#pragma endregion

#pragma region texture
	unsigned texture_width;
	unsigned texture_height;
	const unsigned* texture;	// 0x00RRGGBB, b is at the higher end
#pragma endregion

	Renderer();

	// three vertices should be transformed properly
	void draw_triangle(my_obj_f face);

	void draw_triangle_edge(my_obj_v a, my_obj_v b, my_obj_v c);

	void draw_line(my_obj_v a, my_obj_v b);

public:
	bool window_exit;

	~Renderer();

	// get class instance
	static Renderer& get(void);

	// create the window, and back_buffer and z_buffer
	int create_window(int width, int height, const TCHAR* title, WNDPROC event_process);

	// close window properties
	void close_window(void);

	// load mesh want show in screen
	void load_mesh(my_obj_elements *mesh);

	// load texture(only one texture in current renderer)
	void load_texture(unsigned width, unsigned height, const unsigned *texture);

	// clear background with black and white block
	void clear(void);

	// Rasterize things to back buffer
	void draw(void);

	// Copy back buffer to front
	void refresh(void);
};

#endif
