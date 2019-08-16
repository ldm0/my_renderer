#pragma once

#ifndef _MY_RENDERER_H_
#define _MY_RENDERER_H_

#include"my_graphic.h"
#include"my_obj_loader/my_obj_loader.h"
#include<Windows.h>

/* GPU Based Software Renderer */

// Remember don't use draw before clear, because result will be overwrite
class Renderer {
public:
    enum DRAW_MODE {
        DRAW_MESH,
        DRAW_COLOR,
        DRAW_MESH_COLOR,
    } draw_mode;

#pragma region camera
    // Initial direction is (0, 0, 1)
    // Assume left hand coordinates
    // Yaw: y, Pitch: x, Roll: z
    vec3 yaw_pitch_roll;
    vec4 camera_position;
#pragma endregion

private:
#pragma region allocators
    // Memory allocation may occur during clipping 
    void *(*renderer_malloc)(size_t);
    void *(*renderer_free)(size_t);
#pragma endregion

#pragma region buffers
    unsigned *back_buffer;
    float *z_buffer;
#pragma endregion

#pragma region mesh
    // Currently only vertex vertex_normal and faces were used
    const my_obj_elements *mesh;
#pragma endregion

#pragma region texture
    unsigned texture_width;
    unsigned texture_height;
    // 0x00RRGGBB, b is at the higher end
    const unsigned* texture;
#pragma endregion

    Renderer();

    

    // three vertices should be transformed properly
    void draw_triangle(my_obj_f face);

    void draw_triangle_edge(my_obj_v a, my_obj_v b, my_obj_v c);

    void draw_line(my_obj_v a, my_obj_v b);

public:
    bool window_exit;

    ~Renderer();

    // Get singleton
    static Renderer& get(void);

    // Set allocator for heap memory allocation during rendering
    void set_allocator(void *(*malloc)(size_t), void (*free)(void *));

    // Create graphic window, with back_buffer and z_buffer
    int create_window(int width, int height, const TCHAR* title, WNDPROC event_process);

    // Close window
    void close_window(void);

    // Load mesh to show
    void load_mesh(const my_obj_elements *mesh);

    // Load texture(shallow copy and only one texture is allowed in current renderer)
    void load_texture(unsigned width, unsigned height, const unsigned *texture);

    // Fill back_buffer with black and gray blocks
    void clear(void);

    // put Rasterize things to back buffer
    void draw(void);

    // Copy back buffer to front
    void refresh(void);
};

#endif
