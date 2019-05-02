#include<stdio.h>
#include<string.h>

int convert_header(FILE* input, FILE* output, unsigned *num_vertices, unsigned *num_faces)
{
	char tmp_string[50];

	do {
		memset(tmp_string, 0, 50);
		if (fscanf(input, "%s", tmp_string) != 1)
			return -1;
	} while (strncmp(tmp_string, "vertex", 6) != 0);
	if (fscanf(input, "%d", num_vertices) != 1)
		return -1;

	do {
		memset(tmp_string, 0, 50);
		if (fscanf(input, "%s", tmp_string) != 1)
			return -1;
	} while (strncmp(tmp_string, "face", 4) != 0);

	if (fscanf(input, "%d", num_faces) != 1)
		return -1;

	if (fprintf(output, "%d %d\n", *num_vertices, *num_faces) <= 0)
		return -1;

	do {
		memset(tmp_string, 0, 50);
		if (fscanf(input, "%s", tmp_string) != 1)
			return -1;
	} while (strncmp(tmp_string, "end_header", 10) != 0);
	return 0;
}

int convert_vertices(FILE* input, FILE* output, unsigned num_vertices)
{
	float x, y, z, confidence, intensity;
	for (unsigned i = 0; i < num_vertices; ++i) {
		if (fscanf(input, "%f %f %f %f %f\n", &x, &y, &z, &confidence, &intensity) != 5)
			return -1;
		if (fprintf(output, "%g %g %g\n", x, y, z) <= 0)
			return -1;
	}
	return 0;
}

int convert_indices(FILE* input, FILE* output, unsigned num_faces)
{
	int num_point, x, y, z;
	for (unsigned i = 0; i < num_faces; ++i) {
		if (fscanf(input, "%d %d %d %d\n", &num_point, &x, &y, &z) != 4)
			return -1;
		if (num_point != 3)
			return -1;
		if (fprintf(output, "%d %d %d\n", x, y, z) <= 0)
			return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	char *file_name = "input.ply";
	if (argc > 1)
		file_name = argv[1];
	FILE *input = fopen(file_name, "r");
	FILE *output = fopen("output.simple", "w");
	unsigned num_vertices, num_faces;
	if (convert_header(input, output, &num_vertices, &num_faces) == -1) {
		fprintf(stderr, "Read file error.\n");
		return -1;
	}
	if (convert_vertices(input, output, num_vertices) == -1) {
		fprintf(stderr, "Convert vertices error.\n");
		return -1;
	}
	if (convert_indices(input, output, num_faces) == -1) {
		fprintf(stderr, "Convert indices error.\n");
		return -1;
	}
	fclose(output);
	fclose(input);
	printf("Convert successfully, result in output.simple");
	return 0;
}
