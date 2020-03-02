#include"my_renderer.h"
#include"my_graphic.h"
#include"my_obj_loader/my_obj_loader.h"
#include"SCG/scg.h"
#include<Windows.h>
#include<tchar.h>
#include<math.h>

Renderer::Renderer()
    :draw_mode(DRAW_MESH),
    yaw_pitch_roll({0}),
    camera_position({0}),
    back_buffer(0),
    z_buffer(0),
    v_buffer(0),
    c_buffer(0),
    mesh(0),
    /*
    texture_width(0),
    texture_height(0),
    texture(0),
    */
    //window_exit(false),
    alloc(0),
    drop(0)
{
}

Renderer::~Renderer()
{
    if (mesh) {
        drop(v_buffer);
        drop(c_buffer);
    }
    mesh = NULL;
}

void Renderer::v_shading(void)
{
    if (draw_mode == DRAW_MESH)
        return;
    unsigned numf = mesh->num_f;
    for (unsigned i = 0; i < numf; ++i) {
        //float brightness = 0.1;
        c_buffer[i].x = 0xff0000ff;
        c_buffer[i].y = 0x00ff00ff;
        c_buffer[i].z = 0x0000ffff;
    }
}

void Renderer::projection(void)
{
    unsigned numv = mesh->num_v;
    for (unsigned i = 0; i < numv; ++i) {
        f4 tmp_v = {
            mesh->v_buffer[i].x,
            mesh->v_buffer[i].y,
            mesh->v_buffer[i].z,
            1.f
        };
        tmp_v = view_space(&tmp_v, &camera_position, &yaw_pitch_roll);
        v_buffer[i].x = tmp_v.x;
        v_buffer[i].y = tmp_v.y;
        v_buffer[i].z = tmp_v.z;
    }
}

void Renderer::clipping(void)
{
    // do nothing
}

void Renderer::screen_mapping(void)
{
    int width = scg_window_width;
    int height = scg_window_height;
    unsigned numv = mesh->num_v;
    for (unsigned i = 0; i < numv; ++i) {
        float scale = fminf((float)width, (float)height);
        v_buffer[i].x = v_buffer[i].x * scale + .5f * width;
        // Negative sign for origin at top-left corner
        v_buffer[i].y = -v_buffer[i].y * scale + .5f * height;
    }
}

void Renderer::draw_line(f3 a, f3 b)
{
    int width = scg_window_width;
    int height = scg_window_height;

    // z buffer is not needed when drawing line
    float a_b_x = a.x - b.x;
    float a_b_y = a.y - b.y;
    float a_b_z = a.z - b.z;
    unsigned line_color = 0x00ffffff;
    if (fabsf(a_b_x) < fabsf(a_b_y)) {
        int top = (int)clampf(fminf(a.y, b.y), 0, (float)height);
        int bottom = (int)clampf(fmaxf(a.y, b.y), 0, (float)height);
        for (int y = top; y < bottom; ++y) {
            float depth = 1 / (a.z - a_b_z * (a.y - y) / a_b_y);
            int x = (int)((b.x * (a.y - (float)y) + a.x * ((float)y - b.y)) / a_b_y);
            if (x >= 0 && x < width) {
                if (depth >= z_buffer[x + y * width]) {
                    z_buffer[x + y * width] = depth;
                    back_buffer[x + y * width] = line_color;
                }
            }
        }
    } else {
        int left = (int)clampf(fminf(a.x, b.x), 0, (float)width);
        int right = (int)clampf(fmaxf(a.x, b.x), 0, (float)width);
        for (int x = left; x < right; ++x) {
            float depth = 1 / (a.z - a_b_z * (a.x - x) / a_b_x);
            int y = (int)((b.y * (a.x - (float)x) + a.y * ((float)x - b.x)) / a_b_x);
            if (y >= 0 && y < height) {
                if (depth >= z_buffer[x + y * width]) {
                    z_buffer[x + y * width] = depth;
                    back_buffer[x + y * width] = line_color;
                }
            }
        }
    }
}


void Renderer::rasterization(void)
{
    int width = scg_window_width;
    int height = scg_window_height;

    unsigned numf = mesh->num_f;
    for (unsigned i = 0; i < numf; ++i) {
        u3 triangle = {
            mesh->f_buffer[i].v1,
            mesh->f_buffer[i].v2,
            mesh->f_buffer[i].v3,
        };
        f3 a = v_buffer[triangle.x - 1];
        f3 b = v_buffer[triangle.y - 1];
        f3 c = v_buffer[triangle.z - 1];

        if (draw_mode != DRAW_MESH) {
            // Draw color
            float window_height_f = (float)height;
            float window_width_f = (float)width;
            // get scan area
            float left_f    = clampf(fminf(fminf(a.x, b.x), c.x), 0.f, window_width_f);
            float right_f   = clampf(fmaxf(fmaxf(a.x, b.x), c.x), 0.f, window_width_f);
            float top_f     = clampf(fminf(fminf(a.y, b.y), c.y), 0.f, window_height_f);
            float bottom_f  = clampf(fmaxf(fmaxf(a.y, b.y), c.y), 0.f, window_height_f);

            int left = (int)(left_f + .5f);
            int right = (int)(right_f + .5f);
            int top = (int)(top_f + .5f);
            int bottom = (int)(bottom_f + .5f);

            float b_c_y = (b.y - c.y);
            float c_a_y = (c.y - a.y);
            float a_b_y = (a.y - b.y);

            int a_color_r = c_buffer[i].x >> 24;
            int a_color_g = 0xff & (c_buffer[i].x >> 16);
            int a_color_b = 0xff & (c_buffer[i].x >> 8);
            int b_color_r = c_buffer[i].y >> 24;
            int b_color_g = 0xff & (c_buffer[i].y >> 16);
            int b_color_b = 0xff & (c_buffer[i].y >> 8);
            int c_color_r = c_buffer[i].z >> 24;
            int c_color_g = 0xff & (c_buffer[i].z >> 16);
            int c_color_b = 0xff & (c_buffer[i].z >> 8);

            float b_a_z = b.z - a.z;
            float c_a_z = c.z - a.z;

            // Coordinate transform
            // Build two vector
            f2 b_a_position;
            b_a_position.x = b.x - a.x;
            b_a_position.y = -a_b_y;
            float length1 = sqrtf(b_a_position.x * b_a_position.x + b_a_position.y * b_a_position.y);
            b_a_position.x /= length1;
            b_a_position.y /= length1;
            f2 c_a_position;
            c_a_position.x = c.x - a.x;
            c_a_position.y = c_a_y;
            float length2 = sqrtf(c_a_position.x * c_a_position.x + c_a_position.y * c_a_position.y);
            c_a_position.x /= length2;
            c_a_position.y /= length2;

            // Build transform 
            float determinant = b_a_position.x * c_a_position.y - b_a_position.y * c_a_position.x;
            mat2x2 inv_coord_transform;
            inv_coord_transform.m[0][0] = c_a_position.y / determinant;
            inv_coord_transform.m[0][1] = -b_a_position.y / determinant;
            inv_coord_transform.m[1][0] = -c_a_position.x / determinant;
            inv_coord_transform.m[1][1] = b_a_position.x / determinant;
            float tmp = b_c_y * a.x + c_a_y * b.x + a_b_y * c.x;

            for (unsigned y = top; y < bottom; ++y) {
                for (unsigned x = left; x < right; ++x) {
                    float x_f = (float)x;
                    float y_f = (float)y;
                    float side0 = b_c_y * x_f
                                + (c.y - y_f) * b.x
                                + (y_f - b.y) * c.x;

                    float side1 = (y_f - c.y) * a.x
                                + c_a_y * x_f
                                + (a.y - y_f) * c.x;

                    float side2 = (b.y - y_f) * a.x
                                + (y_f - a.y) * b.x
                                + a_b_y * x_f;

                    // the point is same side with another triangle point in each in 3 side
                    bool draw = (tmp * side0 >= -0.f) && (tmp * side1 >= -0.f) && (tmp * side2 >= -0.f);
                    if (!draw)
                        continue;
                    float u = ((x_f - a.x) * inv_coord_transform.m[0][0] + (y_f - a.y) * inv_coord_transform.m[1][0]) / length1;
                    float v = ((x_f - a.x) * inv_coord_transform.m[0][1] + (y_f - a.y) * inv_coord_transform.m[1][1]) / length2;
                    // Depth
                    float pixel_depth = 1 / (a.z + u * b_a_z + v * c_a_z);
                    if (pixel_depth > z_buffer[x + y * width]) {
                        z_buffer[x + y * width] = pixel_depth;
                        float tmp = 1 - u - v;
                        unsigned pixel_color_r = (unsigned)(tmp * a_color_r + u * b_color_r + v * c_color_r); 
                        unsigned pixel_color_g = (unsigned)(tmp * a_color_g + u * b_color_g + v * c_color_g); 
                        unsigned pixel_color_b = (unsigned)(tmp * a_color_b + u * b_color_b + v * c_color_b); 
                        unsigned pixel_color = (unsigned)((pixel_color_r << 24) | (pixel_color_g << 16) | (pixel_color_b << 8));
                        back_buffer[x + y * width] = pixel_color >> 8;
                    }
                }
            }
        }

        if (draw_mode != DRAW_COLOR) {
            // Draw mesh
            draw_line(a, b);
            draw_line(b, c);
            draw_line(c, a);
        }
    }
}

Renderer& Renderer::get(void)
{
    static Renderer instance;
    return instance;
}

void Renderer::set_allocator(void *(*_malloc)(size_t), void (*_free)(void *))
{
    this->alloc = _malloc;
    this->drop = _free;
}

int Renderer::create_window(int width, int height, const TCHAR* title, WNDPROC event_process)
{
    width = width;
    height = height;

    if (scg_create_window(width, height, title, event_process))
        return -1;

    back_buffer = scg_back_buffer;

    // create depth buffer with the same width and height
    z_buffer = (float *)alloc(width * height * sizeof(float));
    if (!z_buffer)
        return -1;
    return 0;
}

void Renderer::close_window(void)
{
    if (z_buffer)
        drop(z_buffer);
    z_buffer = NULL;

    scg_close_window();
}

void Renderer::refresh(void)
{
    scg_refresh();
}

void Renderer::clear(void)
{
    int width = scg_window_width;
    int height = scg_window_height;
#define block_size 32 
#define block_color 0x7f
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            unsigned b_color = (((x / block_size) % 2) ^ ((y / block_size) % 2)) * block_color;
            back_buffer[x + y * width] = b_color << 16 | b_color << 8 | b_color;
            z_buffer[x + y * width] = 0.f;
        }
    }
#undef block_color
#undef block_size
}

int Renderer::load_mesh(const my_obj_elements *_mesh)
{
    mesh = _mesh;

    if (v_buffer)
        drop(v_buffer);
    v_buffer = (f3 *)alloc(_mesh->num_v * sizeof(f3));
    if (!v_buffer)
        return -1;

    if (c_buffer)
        drop(c_buffer);
    c_buffer = (u3 *)alloc(_mesh->num_f * sizeof(u3));
    if (!c_buffer)
        return -1;

    return 0;
}

/*
void Renderer::load_texture(unsigned width, unsigned height, const unsigned *tex)
{
    texture_width = width;
    texture_height = height;
    texture = tex;
}
*/

void Renderer::draw(void)
{
    v_shading();
    projection();
    clipping();
    screen_mapping();
    rasterization();
}
