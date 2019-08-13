#include"my_renderer.h"
#include"my_graphic.h"
#include"my_obj_loader/my_obj_loader.h"
#include"SCG/scg.h"
#include<Windows.h>
#include<stdlib.h>		// for malloc
#include<tchar.h>
#include<math.h>		// for fmax

Renderer::Renderer()
	:draw_mode(DRAW_MESH_COLOR),
	yaw_pitch_roll({0}),
	camera_position({0}),
	back_buffer(0),
	z_buffer(0),
	mesh(0),
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
	scg_window_width = width;
	scg_window_height = height;

	if (scg_create_window(width, height, title, event_process))
		return -1;

	back_buffer = scg_back_buffer;

	// create depth buffer with the same width and height
	z_buffer = (float *)malloc((unsigned long long)scg_window_width * (unsigned long long)scg_window_height * sizeof(float));
	if (!z_buffer)
		return -1;

	return 0;
}

void Renderer::close_window(void)
{
	if (z_buffer)
		free(z_buffer);

	scg_close_window();
}

void Renderer::refresh(void)
{
	scg_refresh();
}

void Renderer::clear(void)
{
#define block_size 32 
#define block_color 0x7f
	for (int y = 0; y < scg_window_height; ++y) {
		for (int x = 0; x < scg_window_width; ++x) {
			int b_color = (((x / block_size) % 2) ^ ((y / block_size) % 2)) * block_color;
			back_buffer[x + y * scg_window_width] = b_color << 16 | b_color << 8 | b_color;
			z_buffer[x + y * scg_window_width] = 0.f;
		}
	}
#undef block_color
#undef block_size
}

void Renderer::load_mesh(const my_obj_elements *_mesh)
{
	mesh = _mesh;
}

void Renderer::load_texture(unsigned width, unsigned height, const unsigned *tex)
{
	texture_width = width;
	texture_height = height;
	texture = tex;
}

void Renderer::draw(void)
{
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
		float scale = fminf((float)scg_window_width, (float)scg_window_height);
		a.position.x = a.position.x * scale + .5f * scg_window_width;
		b.position.x = b.position.x * scale + .5f * scg_window_width;
		c.position.x = c.position.x * scale + .5f * scg_window_width;
		// negative sign for origin at top-left corner
		a.position.y = -a.position.y * scale + .5f * scg_window_height;
		b.position.y = -b.position.y * scale + .5f * scg_window_height;
		c.position.y = -c.position.y * scale + .5f * scg_window_height;

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
	float a_b_z = a.position.z - b.position.z;
	unsigned line_color = 0x00ffffff;
	if (fabsf(a_b_x) < fabsf(a_b_y)) {
		int top = (int)clampf(fminf(a.position.y, b.position.y), 0, (float)scg_window_height);
		int bottom = (int)clampf(fmaxf(a.position.y, b.position.y), 0, (float)scg_window_height);
		for (int y = top; y < bottom; ++y) {
			float depth = 1 / (a.position.z - a_b_z * (a.position.y - y) / a_b_y);
			int x = (int)((b.position.x * (a.position.y - (float)y) + a.position.x * ((float)y - b.position.y)) / a_b_y);
			if (x >= 0 && x < scg_window_width) {
				if (depth >= z_buffer[x + y * scg_window_width]) {
					z_buffer[x + y * scg_window_width] = depth;
					back_buffer[x + y * scg_window_width] = line_color;
				}
			}
		}
	} else {
		int left = (int)clampf(fminf(a.position.x, b.position.x), 0, (float)scg_window_width);
		int right = (int)clampf(fmaxf(a.position.x, b.position.x), 0, (float)scg_window_width);
		for (int x = left; x < right; ++x) {
			float depth = 1 / (a.position.z + a_b_z * (a.position.x - x) / a_b_x);
			int y = (int)((b.position.y * (a.position.x - (float)x) + a.position.y * ((float)x - b.position.x)) / a_b_x);
			if (y >= 0 && y < scg_window_height) {
				if (depth >= z_buffer[x + y * scg_window_width]) {
					z_buffer[x + y * scg_window_width] = depth;
					back_buffer[x + y * scg_window_width] = line_color;
				}
			}
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
	float window_height_f = (float)scg_window_height;
	float window_width_f = (float)scg_window_width;
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
		+b_c_y * a.position.x
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
				+b_c_y * x_f
				+ (c.position.y - y_f) * b.position.x
				+ (y_f - b.position.y) * c.position.x;
			float side1 =
				+(y_f - c.position.y) * a.position.x
				+ c_a_y * x_f
				+ (a.position.y - y_f) * c.position.x;
			float side2 =
				+(b.position.y - y_f) * a.position.x
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
				if (pixel_depth > z_buffer[x + y * scg_window_width]) {
					z_buffer[x + y * scg_window_width] = pixel_depth;
					unsigned pixel_color = (unsigned)(a.position.y + u * b_a_color + v * c_a_color);
					back_buffer[x + y * scg_window_width] = pixel_color << 8;
				}
			}
		}
	}
}

