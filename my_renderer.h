#pragma once

#ifndef _MY_RENDERER_H_
#define _MY_RENDERER_H_

#include"my_graphic.h"
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
	// special structures
	struct Vertex { vec4 position; };
	struct Face { unsigned x, y, z; };

#pragma region camera
	// initial direction is fixed to (0, 0, 1)
	// Assume left hand coordinates
	// yaw: y, pitch: x, roll: z
	vec3 yaw_pitch_roll;
	vec4 camera_position;
#pragma endregion

private:
#pragma region window
	int window_width;
	int window_height;
	const TCHAR* window_title;
	HWND window_handle;
	WNDCLASS window_class;
	ATOM window_class_atom;

	// The device context of second buffer.
	HDC back_buffer_dc;

	// Working DIB buffer in the back, belongs to second_buffer_dc.
	// It is being write and being block transferred to main window description each frame.
	HBITMAP back_buffer_handle;

	// old DIB buffer created with screen_buffer_dc, 
	// store to release in the end. 
	HBITMAP back_buffer_original_handle;
#pragma endregion

#pragma region buffers
	unsigned* back_buffer;
	float *z_buffer;
#pragma endregion

#pragma region mesh
	unsigned mesh_vertices_length;
	const Vertex* mesh_vertices;
	unsigned mesh_faces_length;
	const Face* mesh_faces;
#pragma endregion

#pragma region texture
	unsigned texture_width;
	unsigned texture_height;
	const unsigned* texture;	// 0x00RRGGBB, b is at the higher end
#pragma endregion

	Renderer();

	// three vertices should be transformed properly
	void draw_triangle(Vertex a, Vertex b, Vertex c);

	void draw_triangle_edge(Vertex a, Vertex b, Vertex c);

	void draw_line(Vertex a, Vertex b);

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
	void load_mesh(const Vertex* mesh, unsigned mesh_length, const Face *mesh_faces, unsigned mesh_faces_length);

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
