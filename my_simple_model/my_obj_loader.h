#pragma once

#ifndef _MY_OBJ_LOADER_H_
#define _MY_OBJ_LOADER_H_

#include<stdio.h>

typedef struct my_obj_vec3 {
    float x, y, z;
} my_obj_vec3;

int my_obj_get_num_elements(
    FILE *obj_file,
    unsigned *num_v, 
    unsigned *num_vt, 
    unsigned *num_vn, 
    unsigned *num_vp,
    unsigned *num_f,
    unsigned *num_l);

int my_obj_get_v(
    FILE *obj_file,
    unsigned v_buffer_length, 
    my_obj_vec3 *v_buffer);

// currently w parameter of texture is ignored
int my_obj_get_vt(
    FILE *obj_file,
    unsigned vt_buffer_length, 
    my_obj_vec3 *vt_buffer);

int my_obj_get_vn(
    FILE *obj_file,
    unsigned vn_buffer_length,
    my_obj_vec3 *vn_buffer);

int my_obj_get_vp(
    FILE *obj_file,
    unsigned vp_buffer_length,
    my_obj_vec3 *vn_buffer);

#endif