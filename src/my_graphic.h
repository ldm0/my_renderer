#pragma once

#ifndef _MY_GRAPHIC_H_
#define _MY_GRAPHIC_H_

#include<math.h>

typedef struct mat2x2 { float m[2][2]; } mat2x2;
typedef struct mat4x4 { float m[4][4]; } mat4x4;
typedef struct f2 { float x, y; } f2;
typedef struct f3 { float x, y, z; } f3;
typedef struct f4 { float x, y, z, w; } f4;
typedef struct u3 { unsigned x, y, z; } u3;

inline int ceiling(float f)
{
    // From testing, (int)(3.0f + .999999f) = 3
    return (int)(f + .999999);
}

inline float clampf(float x, float min_val, float max_val)
{
    return fminf(max_val, fmaxf(min_val, x));
}

inline void negative(mat4x4 * matrix)
{
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            matrix->m[y][x] *= -1.f;
        }
    }
}

// this cross product assume a and b all direction 
// and two parameter w are all 0.f
inline f4 cross_product(const f4 *a, const f4 *b)
{
    f4 result;
    result.x = a->y * b->z - a->z * b->y;
    result.y = a->z * b->x - a->x * b->z;
    result.z = a->x * b->y - a->y * b->x;
    result.w = 0.f;
    return result;
}

inline f4 normalize_vec4(const f4 *a)
{
    f4 result;
    float base = sqrtf(a->x * a->x + a->y + a->y + a->z + a->z);
    result.x = a->x / base;
    result.y = a->y / base;
    result.z = a->z / base;
    result.w = a->w;
    return result;
}

inline f2 normalize_vec2(const f2 *a)
{
    f2 result;
    float base = sqrtf(a->x * a->x + a->y + a->y);
    result.x = a->x / base;
    result.y = a->y / base;
    return result;
}

inline f4 translate(const f4 *vertex_position, const f4 * vector, float ratio)
{
    f4 result = *vertex_position;
    result.x += ratio * vector->x;
    result.y += ratio * vector->y;
    result.z += ratio * vector->z;
    return result;
}

inline f4 rotate(const f4 * vertex_position, const f3 * yaw_pitch_roll)
{
    // rotation
    float sin_yaw = sinf(yaw_pitch_roll->x);
    float cos_yaw = cosf(yaw_pitch_roll->x);
    float sin_pitch = sinf(yaw_pitch_roll->y);
    float cos_pitch = cosf(yaw_pitch_roll->y);
    float sin_roll = sinf(yaw_pitch_roll->z);
    float cos_roll = cosf(yaw_pitch_roll->z);

    f4 before = *vertex_position;
    f4 after = *vertex_position;

    // roll rotate
    after.x = before.x * cos_roll - before.y * sin_roll;
    after.y = before.x * sin_roll + before.y * cos_roll;
    before = after;

    // pitch rotate 
    after.y = before.y * cos_pitch - before.z * sin_pitch;
    after.z = before.y * sin_pitch + before.z * cos_pitch;
    before = after;

    // yaw rotate
    after.x = before.x * cos_yaw - before.z * sin_yaw;
    after.z = before.x * sin_yaw + before.z * cos_yaw;

    return after;
}

inline f4 rotate_back(const f4 * vertex_position, const f3 * yaw_pitch_roll)
{
    // rotation
    float sin_yaw = sinf(yaw_pitch_roll->x);
    float cos_yaw = cosf(yaw_pitch_roll->x);
    float sin_pitch = sinf(yaw_pitch_roll->y);
    float cos_pitch = cosf(yaw_pitch_roll->y);
    float sin_roll = sinf(yaw_pitch_roll->z);
    float cos_roll = cosf(yaw_pitch_roll->z);

    f4 before = *vertex_position;
    f4 after = *vertex_position;

    // yaw rotate back
    after.x = before.x * cos_yaw + before.z * sin_yaw;
    after.z = before.x * -sin_yaw + before.z * cos_yaw;
    before = after;

    // pitch rotate back 
    after.y = before.y * cos_pitch + before.z * sin_pitch;
    after.z = before.y * -sin_pitch + before.z * cos_pitch;
    before = after;

    // roll rotate back
    after.x = before.x * cos_roll + before.y * sin_roll;
    after.y = before.x * -sin_roll + before.y * cos_roll;

    return after;
}

inline f4 view_space(const f4 * vertex_position, const f4 * camera_position, const f3 * yaw_pitch_roll)
{
    // translate
    f4 result = translate(vertex_position, camera_position, -1.f);

    // rotation
    result = rotate_back(&result, yaw_pitch_roll);

    // perspective
    // assume default camera direction is (0, 0, 1, 0)
    result.x /= result.z;
    result.y /= result.z;
    return result;
}

#endif

