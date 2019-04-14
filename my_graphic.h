#pragma once

#ifndef _MY_GRAPHIC_H_
#define _MY_GRAPHIC_H_

#include<math.h>

typedef struct mat4x4 { float m[4][4]; } mat4x4;
typedef struct vec2 { float x, y; } vec2;
typedef struct vec3 { float x, y, z; } vec3;
typedef struct vec4 { float x, y, z, w; } vec4;

inline int ceiling(float f)
{
	// From testing, (int)(3.0f + .999999f) = 3
	return (int)(f + .999999);
}

inline float clampf(float x, float minVal, float maxVal)
{
	return fminf(maxVal, fmaxf(minVal, x));
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
inline vec4 cross_product(const vec4 *a, const vec4 *b)
{
	vec4 result;
	result.x = a->y * b->z - a->z * b->y;
	result.y = a->z * b->x - a->x * b->z;
	result.z = a->x * b->y - a->y * b->x;
	result.w = 0.f;
	return result;
}

inline vec4 normalize(const vec4 *a)
{
	vec4 result;
	float base = sqrtf(a->x * a->x + a->y + a->y + a->z + a->z);
	result.x = a->x / base;
	result.y = a->y / base;
	result.z = a->z / base;
	result.w = a->w;
	return result;
}

inline vec4 translate(const vec4 *vertex_position, const vec4 * vector, float ratio)
{
	vec4 result = *vertex_position;
	result.x += ratio * vector->x;
	result.y += ratio * vector->y;
	result.z += ratio * vector->z;
	return result;
}

inline vec4 rotate(const vec4 * vertex_position, const vec3 * yaw_pitch_roll)
{
	// rotation
	float sin_yaw = sinf(yaw_pitch_roll->x);
	float cos_yaw = cosf(yaw_pitch_roll->x);
	float sin_pitch = sinf(yaw_pitch_roll->y);
	float cos_pitch = cosf(yaw_pitch_roll->y);
	float sin_roll = sinf(yaw_pitch_roll->z);
	float cos_roll = cosf(yaw_pitch_roll->z);

	vec4 before = *vertex_position;
	vec4 after = *vertex_position;		

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

inline vec4 rotate_back(const vec4 * vertex_position, const vec3 * yaw_pitch_roll)
{
	// rotation
	float sin_yaw = sinf(yaw_pitch_roll->x);
	float cos_yaw = cosf(yaw_pitch_roll->x);
	float sin_pitch = sinf(yaw_pitch_roll->y);
	float cos_pitch = cosf(yaw_pitch_roll->y);
	float sin_roll = sinf(yaw_pitch_roll->z);
	float cos_roll = cosf(yaw_pitch_roll->z);

	vec4 before = *vertex_position;
	vec4 after = *vertex_position;		

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

inline vec4 view_space(const vec4 * vertex_position, const vec4 * camera_position, const vec3 * yaw_pitch_roll)
{
	// translate
	vec4 result = translate(vertex_position, camera_position, -1.f);

	// rotation
	result = rotate_back(&result, yaw_pitch_roll);

	// perspective
	// assume default camera direction is (0, 0, 1, 0)
	result.x /= result.z;
	result.y /= result.z;
	return result;
}

#endif

