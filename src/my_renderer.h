#pragma once

#ifndef _MY_RENDERER_H_
#define _MY_RENDERER_H_

#include"my_graphic.h"
#include"my_obj_loader/my_obj_loader.h"
#include<Windows.h>

/* GPU Based Software Renderer */

// Remember don't use draw before clear, because result will be overwrote
class Renderer {
public:
    enum DRAW_MODE {
        DRAW_MESH,
        DRAW_COLOR,
        DRAW_MESH_COLOR,
    } draw_mode;

    // Initial direction is (0, 0, 1)
    // Assume left hand coordinates
    // Yaw: y, Pitch: x, Roll: z
    f3 yaw_pitch_roll;
    f4 camera_position;

private:
    // Memory allocation may occur during clipping 
    void*(*alloc)(size_t);
    void (*drop)(void *);

    // Color back buffer
    unsigned *back_buffer;
    // Each pixel in window occupies a z value
    float *z_buffer;
    // Buffer for vertices to be projected and screen mapping
    f3 *v_buffer;
    // Color of three vertices in each face
    // 0x00RRGGBB, B is the big end
    u3 *c_buffer;

    // Currently only vertex vertex_normal and faces were used
    const my_obj_elements *mesh;

    /*
    unsigned texture_width;
    unsigned texture_height;
    // 0x00RRGGBB, B is the big end
    const unsigned* texture;
    */

    Renderer();

    void v_shading(void);
    void projection(void);
    void clipping(void);
    void screen_mapping(void);
    void rasterization(void);
    void draw_line(f3 a, f3 b);

public:
    //bool window_exit;

    ~Renderer();

    // Get singleton
    static Renderer& get(void);

    // Set allocator for heap memory allocation during rendering
    void set_allocator(void *(*alloc)(size_t), void (*drop)(void *));

    // Create graphic window, with back_buffer and z_buffer
    int create_window(int width, int height, const TCHAR* title, WNDPROC event_process);

    // Close window
    void close_window(void);

    // Load mesh to show and malloc enough buffer for projection and vertex shading(light)
    int load_mesh(const my_obj_elements *mesh);

    // Load texture(shallow copy and only one texture is allowed in current renderer)
    //void load_texture(unsigned width, unsigned height, const unsigned *texture);

    // Fill back_buffer with black and gray blocks
    void clear(void);

    // Draw images to back_buffer
    void draw(void);

    // Swap buffers
    void refresh(void);
};

#endif
