#pragma once

#ifndef _MY_OBJ_LOADER_H_
#define _MY_OBJ_LOADER_H_

#include<stdio.h>

// vertices x, y, z
typedef struct my_obj_v {
	float x, y, z;
} my_obj_v;

// texture coordinates
typedef struct my_obj_vt {
	float u, v;
} my_obj_vt;

// vertex normal
typedef struct my_obj_vn {
	float x, y, z;
} my_obj_vn;

// Parameter space vertices
typedef struct my_obj_vp {
	float u, v, w;
} my_obj_vp;

typedef struct my_obj_f {
	unsigned v1, v2, v3;
	unsigned vt1, vt2, vt3;
	unsigned vn1, vn2, vn3;
} my_obj_f;

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
	void* (*func_realloc)(void *memory, size_t size), 
	void (*func_free)(void* memory));

int my_obj_get_elements(
	char *file_name,
	my_obj_elements *elements);

void my_obj_release_elements( 
	my_obj_elements *elements);

#endif