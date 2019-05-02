#pragma once
#ifndef _MY_SIMPLE_MODEL_LOADER_H_
#define _MY_SIMPLE_MODEL_LOADER_H_

#define _CRT_SECURE_NO_WARNINGS
typedef struct {
	float x, y, z, w;
} simple_vertex;

typedef struct {
	unsigned x, y, z;
} simple_face;

#include<stdio.h>
int read_simple_model_header(FILE *simple_model, unsigned *num_vertices, unsigned *num_faces);
int read_simple_model_mesh_vertices(FILE *simple_model, unsigned num_vertices, simple_vertex *vertex_array);
int read_simple_model_mesh_faces(FILE *simple_model, unsigned num_faces, simple_face *face_array);

#endif