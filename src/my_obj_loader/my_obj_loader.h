#pragma once

#ifndef _MY_OBJ_LOADER_H_
#define _MY_OBJ_LOADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>

// vertices pool
typedef struct my_obj_v {
	float x, y, z;
} my_obj_v;

// texture coordinates pool
typedef struct my_obj_vt {
	float u, v;
} my_obj_vt;

// vertex normal pool
typedef struct my_obj_vn {
	float x, y, z;
} my_obj_vn;

// parameter space vertices (sounds good but I don't know how it's used. :P)
typedef struct my_obj_vp {
	float u, v, w;
} my_obj_vp;

// faces members are all pointers points to pool before (member can be -1 which indicates its empty)
typedef struct my_obj_f {
	int v1, v2, v3;
	int vt1, vt2, vt3;
	int vn1, vn2, vn3;
} my_obj_f;

// line (vertex pointer array)
typedef struct my_obj_l {
	unsigned size;
	unsigned *v;
} my_obj_l;

typedef struct my_obj_elements {
	unsigned num_v;
	my_obj_v *v_buffer;
	unsigned num_vt;
	my_obj_vt *vt_buffer;
	unsigned num_vn;
	my_obj_vn *vn_buffer;
	unsigned num_vp;
	my_obj_vp *vp_buffer;
	unsigned num_f;
	my_obj_f *f_buffer;
	unsigned num_l;
	my_obj_l *l_buffer;
} my_obj_elements;

void my_obj_set_allocator(
	void* (*func_malloc)(size_t size),
	void (*func_free)(void* memory));

int my_obj_get_mesh(
	const char *file_name,
	my_obj_elements *elements);

void my_obj_free_mesh( 
	my_obj_elements *elements);

#ifdef __cplusplus
}
#endif

#endif