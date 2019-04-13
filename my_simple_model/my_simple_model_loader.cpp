#include "my_simple_model_loader.h"
#include<stdio.h>

int read_simple_model_header(FILE * simple_model, unsigned * num_vertices, unsigned * num_faces)
{
	if (fscanf(simple_model, "%d %d\n", num_vertices, num_faces) != 2)
		return -1;
	return 0;
}

int read_simple_model_mesh_vertices(FILE * simple_model, unsigned num_vertices, simple_vertex *vertices)
{
	for (unsigned i = 0; i < num_vertices; ++i) {
		if (fscanf(simple_model, "%f %f %f\n", &vertices[i].x, &vertices[i].y, &vertices[i].z) != 3)
			return -1; 
		vertices[i].w = 1.f;
	}
	return 0;
}

int read_simple_model_mesh_faces(FILE * simple_model, unsigned num_faces, simple_face *faces)
{
	for (unsigned i = 0; i < num_faces; ++i)
		if (fscanf(simple_model, "%d %d %d\n", &faces[i].x, &faces[i].y, &faces[i].z) != 3)
			return -1; 
	return 0;
}
