#include"my_renderer.h"
#include"my_graphic.h"
#include<Windows.h>
#include<stdlib.h>		// for malloc
#include<tchar.h>
#include<math.h>		// for fmax

Renderer::Renderer()
	:draw_mode(DRAW_MESH_COLOR),
	window_width(0),
	window_height(0),
	window_title(0),
	window_handle(0),
	window_class({0}),
	window_class_atom(0),
	back_buffer_dc(0),
	back_buffer_handle(0),
	back_buffer_original_handle(0),
	back_buffer(0),
	z_buffer(0),
	camera_position({0}),
	yaw_pitch_roll({0}),
	mesh_faces_length(0),
	mesh_faces(0),
	mesh_vertices_length(0),
	mesh_vertices(0),
	texture_width(0),
	texture_height(0),
	texture(0),
	window_exit(false)
{
}

Renderer::~Renderer()
{
	close_window();
}

Renderer& Renderer::get(void)
{
	static Renderer instance;
	return instance;
}

int Renderer::create_window(int width, int height, const TCHAR* title, WNDPROC event_process)
{
	// classic window and cannot be resized
	DWORD window_style = WS_OVERLAPPEDWINDOW & (~WS_THICKFRAME);

	window_title = title;

	window_class.style = CS_BYTEALIGNWINDOW | CS_BYTEALIGNCLIENT;
	window_class.lpfnWndProc = event_process;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 0;
	window_class.hInstance = 0;
	window_class.hIcon = NULL;
	window_class.hCursor = LoadCursor(0, IDC_ARROW);
	window_class.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	window_class.lpszMenuName = NULL;
	window_class.lpszClassName = _T("MainWndClass");
	window_class_atom = RegisterClass(&window_class);
	if (!window_class_atom)
		return -1;

	window_width = width;
	window_height = height;
	RECT tmp_rect = {0, 0, width, height};
	AdjustWindowRect(&tmp_rect, window_style, 0);
	// width and height plus window frame width height
	long tmp_window_width = tmp_rect.right - tmp_rect.left;
	long tmp_window_height = tmp_rect.bottom - tmp_rect.top;
	window_handle = CreateWindow(
		(LPCTSTR)window_class_atom,
		title,
		window_style,
		(GetSystemMetrics(SM_CXSCREEN) - tmp_window_width) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - tmp_window_height) / 2,
		tmp_window_width,
		tmp_window_height,
		NULL,
		NULL,
		window_class.hInstance,
		NULL);
	if (!window_handle)
		return -1;

	HDC main_dc = GetDC(window_handle);
	back_buffer_dc = CreateCompatibleDC(main_dc);
	ReleaseDC(window_handle, main_dc);

	BITMAPINFO bmp_info = {{0}};
	bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmp_info.bmiHeader.biWidth = window_width;
	bmp_info.bmiHeader.biHeight = -window_height;		// top_down dib section
	bmp_info.bmiHeader.biPlanes = 1;
	bmp_info.bmiHeader.biBitCount = 32;
	bmp_info.bmiHeader.biCompression = BI_RGB;
	bmp_info.bmiHeader.biSizeImage = window_width * window_height * 4;
	bmp_info.bmiHeader.biXPelsPerMeter = 2835;
	bmp_info.bmiHeader.biYPelsPerMeter = 2835;
	bmp_info.bmiHeader.biClrUsed = 0;
	bmp_info.bmiHeader.biClrImportant = 0;
	back_buffer_handle = CreateDIBSection(back_buffer_dc, &bmp_info, DIB_RGB_COLORS, (void**)& back_buffer, 0, 0);
	if (!back_buffer_handle)
		return -1;

	// swap out the old buffer
	back_buffer_original_handle = (HBITMAP)SelectObject(back_buffer_dc, back_buffer_handle);

	// create depth buffer with the same width and height
	z_buffer = (float *)malloc((unsigned long long)window_width * (unsigned long long)window_height * sizeof(float));

	ShowWindow(window_handle, SW_SHOW);
	SetForegroundWindow(window_handle);

	return 0;
}

void Renderer::close_window(void)
{
	if (z_buffer)
		free(z_buffer);

	if (back_buffer_dc) {
		if (back_buffer_original_handle) {
			SelectObject(back_buffer_dc, back_buffer_original_handle);
			DeleteObject(back_buffer_handle);
		}
		DeleteDC(back_buffer_dc);
	}

	if (window_handle)
		CloseWindow(window_handle);

	if (window_class.lpfnWndProc)
		UnregisterClass((LPCSTR)window_class_atom, 0);
}

void Renderer::refresh(void)
{
	HDC main_dc = GetDC(window_handle);
	BitBlt(main_dc, 0, 0, window_width, window_height, back_buffer_dc, 0, 0, SRCCOPY);
	ReleaseDC(window_handle, main_dc);
}

void Renderer::clear(void)
{
#define block_size 32 
#define block_color 0x7f
	for (int y = 0; y < window_height; ++y) {
		for (int x = 0; x < window_width; ++x) {
			int b_color = (((x / block_size) % 2) ^ ((y / block_size) % 2)) * block_color;
			back_buffer[x + y * window_width] = b_color << 16 | b_color << 8 | b_color;
			z_buffer[x + y * window_width] = 0.f;
		}
	}
#undef block_color
#undef block_size
}

void Renderer::load_mesh(const Vertex * _mesh, unsigned _mesh_length, const Face * _mesh_faces, unsigned _mesh_indices_length)
{
	mesh_vertices = _mesh;
	mesh_vertices_length = _mesh_length;
	mesh_faces = _mesh_faces;
	mesh_faces_length = _mesh_indices_length;
}

void Renderer::load_texture(unsigned width, unsigned height, const unsigned *tex)
{
	texture_width = width;
	texture_height = height;
	texture = tex;
}

void Renderer::draw(void)
{
	if (!mesh_vertices_length
		|| !mesh_vertices
		|| !mesh_faces_length
		|| !mesh_faces)
		return;
	for (unsigned i = 0; i < mesh_faces_length; ++i) {
		Vertex a = mesh_vertices[mesh_faces[i].x];
		Vertex b = mesh_vertices[mesh_faces[i].y];
		Vertex c = mesh_vertices[mesh_faces[i].z];

		// Projection
		a.position = view_space(&a.position, &camera_position, &yaw_pitch_roll);
		b.position = view_space(&b.position, &camera_position, &yaw_pitch_roll);
		c.position = view_space(&c.position, &camera_position, &yaw_pitch_roll);

		// Clip

		// Screen mapping
		float scale = fminf((float)window_width, (float)window_height);
		a.position.x = a.position.x * scale + .5f * window_width;
		b.position.x = b.position.x * scale + .5f * window_width;
		c.position.x = c.position.x * scale + .5f * window_width;
		// negative sign for origin at top-left corner
		a.position.y = -a.position.y * scale + .5f * window_height;
		b.position.y = -b.position.y * scale + .5f * window_height;
		c.position.y = -c.position.y * scale + .5f * window_height;

		// Rasterize
		if (draw_mode == DRAW_MESH_COLOR || draw_mode == DRAW_MESH)
			draw_triangle(a, b, c);
		if (draw_mode == DRAW_MESH_COLOR || draw_mode == DRAW_COLOR)
			draw_triangle_edge(a, b, c);
	}
}

void Renderer::draw_line(Vertex a, Vertex b)
{
	// z buffer is not needed when drawing line
	float a_b_x = a.position.x - b.position.x;
	float a_b_y = a.position.y - b.position.y;
	if (fabsf(a_b_x) < fabsf(a_b_y)) {
		int top = (int)clampf(fminf(a.position.y, b.position.y), 0, (float)window_height);
		int bottom = (int)clampf(fmaxf(a.position.y, b.position.y), 0, (float)window_height);
		for (int y = top; y < bottom; ++y) {
			int x = (int)((b.position.x * (a.position.y - (float)y) + a.position.x * ((float)y - b.position.y)) / a_b_y);
			if (x >= 0 && x < window_width)
				back_buffer[x + y * window_width] = 0x00ffffff;
		}
	} else {
		int left = (int)clampf(fminf(a.position.x, b.position.x), 0, (float)window_width);
		int right = (int)clampf(fmaxf(a.position.x, b.position.x), 0, (float)window_width);
		for (int x = left; x < right; ++x) {
			int y = (int)((b.position.y * (a.position.x - (float)x) + a.position.y * ((float)x - b.position.x)) / a_b_x);
			if (y >= 0 && y < window_height)
				back_buffer[x + y * window_width] = 0x00ffffff;
		}
	}
}

void Renderer::draw_triangle_edge(Vertex a, Vertex b, Vertex c)
{
	draw_line(a, b);
	draw_line(b, c);
	draw_line(c, a);
}

void Renderer::draw_triangle(Vertex a, Vertex b, Vertex c)
{
	float window_height_f = (float)window_height;
	float window_width_f = (float)window_width;
	// get scan area
	float left_f = clampf(fminf(fminf(a.position.x, b.position.x), c.position.x), 0.f, window_width_f);
	float right_f = clampf(fmaxf(fmaxf(a.position.x, b.position.x), c.position.x), 0.f, window_width_f);
	float top_f = clampf(fminf(fminf(a.position.y, b.position.y), c.position.y), 0.f, window_height_f);
	float bottom_f = clampf(fmaxf(fmaxf(a.position.y, b.position.y), c.position.y), 0.f, window_height_f);

	int left = (int)left_f;
	int right = (int)right_f;
	int top = (int)top_f;
	int bottom = (int)bottom_f;
	static int n = 0;

	float b_c_y = (b.position.y - c.position.y);
	float c_a_y = (c.position.y - a.position.y);
	float a_b_y = (a.position.y - b.position.y);
	float tmp =
		+ b_c_y * a.position.x
		+ c_a_y * b.position.x
		+ a_b_y * c.position.x;

	// currently just use position_y as color for testing
	// so I don't use c_a_y used before
	float b_a_color = b.position.y - a.position.y;
	float c_a_color = c.position.y - a.position.y;

	float b_a_z = b.position.z - a.position.z;
	float c_a_z = c.position.z - a.position.z;

	// coordinate transform
	vec2 b_a_position;
	b_a_position.x = b.position.x - a.position.x;
	b_a_position.y = -a_b_y;
	float length1 = sqrtf(b_a_position.x * b_a_position.x + b_a_position.y * b_a_position.y);
	b_a_position.x /= length1;
	b_a_position.y /= length1;
	vec2 c_a_position;
	c_a_position.x = c.position.x - a.position.x;
	c_a_position.y = c_a_y;
	float length2 = sqrtf(c_a_position.x * c_a_position.x + c_a_position.y * c_a_position.y);
	c_a_position.x /= length2;
	c_a_position.y /= length2;

	float determinant = b_a_position.x * c_a_position.y - b_a_position.y * c_a_position.x;
	mat2x2 inv_coord_transform;
	inv_coord_transform.m[0][0] = c_a_position.y / determinant;
	inv_coord_transform.m[0][1] = -b_a_position.y / determinant;
	inv_coord_transform.m[1][0] = -c_a_position.x / determinant;
	inv_coord_transform.m[1][1] = b_a_position.x / determinant;

	for (int y = top; y < bottom; ++y) {
		for (int x = left; x < right; ++x) {
			float x_f = (float)x;
			float y_f = (float)y;
			float side0 =
				+ b_c_y * x_f
				+ (c.position.y - y_f) * b.position.x
				+ (y_f - b.position.y) * c.position.x;
			float side1 =
				+ (y_f - c.position.y) * a.position.x
				+ c_a_y * x_f
				+ (a.position.y - y_f) * c.position.x;
			float side2 =
				+ (b.position.y - y_f) * a.position.x
				+ (y_f - a.position.y) * b.position.x
				+ a_b_y * x_f;
			// the point is same side with another triangle point in each in 3 side
			bool inside = (tmp * side0 >= -0.f) && (tmp * side1 >= -0.f) && (tmp * side2 >= -0.f);
			if (inside) {
				float u, v;
				u = ((x_f - a.position.x) * inv_coord_transform.m[0][0] + (y_f - a.position.y) * inv_coord_transform.m[1][0]) / length1;
				v = ((x_f - a.position.x) * inv_coord_transform.m[0][1] + (y_f - a.position.y) * inv_coord_transform.m[1][1]) / length2;
				// depth test
				float pixel_depth = 1 / (a.position.z + u * b_a_z + v * c_a_z);
				if (pixel_depth > z_buffer[x + y * window_width]) {
					z_buffer[x + y * window_width] = pixel_depth;
					unsigned pixel_color = a.position.y + u * b_a_color + v * c_a_color;
					back_buffer[x + y * window_width] = pixel_color << 8;
				}
			}
		}
	}
}

